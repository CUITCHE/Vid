#include "Git.h"
#include <QProcess>
#include <QTextStream>
#include <QVector>
#include <QDir>
#include <git2.h>
#include <QDebug>
#include <git/GitFile.h>

// https://libgit2.github.com/docs/guides/101-samples/

struct GitPrivate: public LoggerI<GitPrivate> {
private:
    git_repository *repo;
public:
    const QString rootDirPath;
    GitPrivate(const QString &rootDirPath)
        : rootDirPath(rootDirPath)
    {
        git_libgit2_init();
        QDir dir(rootDirPath);
        int error = git_repository_open(&repo, rootDirPath.toStdString().c_str());
        if (error < 0) {
            const git_error *e = giterr_last();
            logger->e("Error {}/{}: {}", error, e->klass, e->message);
            exit(error);
        }
    }

    ~GitPrivate() {
        git_repository_free(repo);
        git_libgit2_shutdown();
    }

    static int walk_cb(const char *root,
                      const git_tree_entry *entry,
                      void *payload) {
        GitFile &wd = *static_cast<GitFile *>(payload);
        auto name = git_tree_entry_name(entry);
        git_object_t objtype = git_tree_entry_type(entry);
        qDebug() << "root:" << root << "\t name=" << name << "\t type=" << objtype;
        if (objtype == GIT_OBJECT_TREE) {
            wd.addPath(name, root);
        } else if (objtype == GIT_OBJECT_BLOB) {
            wd.add(name, root);
        }
        return 0;
    }

    void visit(QStringList &dirs, QStringList &files) const {
        git_object *obj = nullptr;
        int err = git_revparse_single(&obj, repo, "HEAD^{tree}");
        git_tree *tree = static_cast<git_tree *>(static_cast<void *>(obj));
        QStringList ret;
        GitFile wd(rootDirPath);
        err = git_tree_walk(tree, GIT_TREEWALK_PRE, GitPrivate::walk_cb, &wd);
        git_object_free(obj);

        wd.visitPath([&](const QString &path) {
           dirs << path;
        });

        wd.visitFiles([&](const QString &filepath) {
            files << filepath;
        });
    }
};

void __go() {
    GitPrivate git("/Users/hejunqiu/Documents/QtProjects/Vid3/");
    QStringList dirs, files;
    git.visit(dirs, files);
    qDebug() << dirs;
    qDebug() << files;
}

Git::Git(const QString &rootDirPath, QObject *parent)
    : QObject(parent)
    , LoggerI(this)
    , d(new GitPrivate(rootDirPath))
{

}

void Git::status(const GitStatusCallback &cb)
{
    QDir dir(d->rootDirPath);
    auto git_dir = QString("--git-dir=%1").arg(dir.filePath(".git"));
    auto work_tree = QString("--work-tree=%1").arg(d->rootDirPath);
    QStringList arguments;

    int val = QProcess::execute("git", arguments << git_dir << work_tree << "add -A");
    if (val == -2 || val == -1) {
        logger->warning("执行git add -A失败");
        return;
    }

    val = QProcess::execute("git", arguments << git_dir << work_tree << "status");
    if (val == -2 || val == -1) {
        logger->warning("执行git status失败");
        return;
    }
    QTextStream ts(stdin);
    auto contents = ts.readAll().splitRef(QString("\n"));

    auto iter = std::find_if(contents.begin(), contents.end(), [](const QStringRef &value) {
        return value == "Changes to be committed:";
    });
    if (iter == contents.end()) {
        return; // no changes.
    }

    QStringList newFiles;
    QStringList modifiedFiles;
    QStringList deletedFiles;

    while (++iter != contents.end()) {
        if (!iter->contains(":")) {
            continue;
        }
        auto components = iter->split(":");
        if (components.size() != 2) {
            logger->d("git status: 非合理line，contents={}", *iter);
        }
        if (components[0].contains("new file")) {
            newFiles << components[1].toString().trimmed();
        } else if (components[0].contains("modified")) {
            modifiedFiles << components[1].toString().trimmed();
        } else if (components[0].contains("deleted")) {
            deletedFiles << components[1].toString().trimmed();
        } else {
            logger->w("Unknown status={}, full={}", components[0], *iter);
        }
    }
    cb(newFiles, modifiedFiles, deletedFiles);
}

const QString &Git::roorDirPath() const
{
    return d->rootDirPath;
}
