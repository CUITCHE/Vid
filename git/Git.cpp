#include "Git.h"
#include <QProcess>
#include <QTextStream>
#include <QVector>
#include <QDir>
#include <git2.h>
#include <QDebug>

// https://libgit2.github.com/docs/guides/101-samples/
class GitGuard {

};

struct GitPrivate: public LoggerI<GitPrivate> {
    const QString rootDirPath;
    git_repository *repo;
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

    static int wal_cb(const char *root,
                      const git_tree_entry *entry,
                      void *payload) {
        QStringList &ret = *(QStringList *)payload;
        auto name = git_tree_entry_name(entry);
        git_object_t objtype = git_tree_entry_type(entry);
        ret << QString("%1 %2").arg(objtype).arg(QString(name));
        qDebug() << "root:" << root << "\t name=" << name << "\t type=" << objtype;
        return 0;
    }

    QStringList visited() const {
        git_object *obj = nullptr;
        int err = git_revparse_single(&obj, repo, "HEAD^{tree}");
        git_tree *tree = (git_tree *)(obj);
        QStringList ret;
        err = git_tree_walk(tree, GIT_TREEWALK_PRE, GitPrivate::wal_cb, &ret);
        git_object_free(obj);
        return ret;
    }
};

void __go() {
    GitPrivate git("/Users/hejunqiu/Documents/QtProjects/Vid3");
    qDebug() << git.visited();
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
