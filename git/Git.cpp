#include "Git.h"
#include <QProcess>
#include <QTextStream>
#include <QVector>
#include <QDir>

Git::Git(const QString &rootDirPath, QObject *parent)
    : QObject(parent)
    , LoggerI(this)
    , _rootDirPath(rootDirPath)
{

}

void Git::status(const GitStatusCallback &cb)
{
    QDir dir(_rootDirPath);
    auto git_dir = QString("--git-dir=%1").arg(dir.filePath(".git"));
    auto work_tree = QString("--work-tree=%1").arg(_rootDirPath);
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
