#include "GitFile.h"
#include <vector>
#include <QStringList>
#include <QQueue>

struct GitDir {
    QString dirName;
    QStringList files;
    GitDir(const QString &name): dirName(name) {}
};

struct GitFilePrivate {
    QString root;
    std::vector<GitDir> dirs;
    GitFilePrivate(const QString &root): root(root) {}
    QStringList rootFiles;
};

GitFile::GitFile(const QString &root)
    : d(new GitFilePrivate(root))
{

}

GitFile::~GitFile() {
    delete d;
}

void GitFile::add(const QString &filename, const QString &root)
{
    if (root.isEmpty()) {
        d->rootFiles << filename;
        return;
    }
    auto iter = std::find_if(d->dirs.begin(), d->dirs.end(), [&](const GitDir &op) {
        return root == op.dirName;
    });
    Q_ASSERT(iter != d->dirs.end());
    iter->files << filename;
}

void GitFile::addPath(const QString &path, const QString &root)
{
    auto rootPath = QString("%1%2/").arg(root).arg(path);

    auto iter = std::find_if(d->dirs.begin(), d->dirs.end(), [&](const GitDir &op) {
        return rootPath == op.dirName;
    });
    Q_ASSERT(iter == d->dirs.end());
    d->dirs.emplace_back(rootPath);
}

void GitFile::visitFiles(const GitFile::FilePathCallBack &cb) const
{
    for (auto &dir: d->dirs) {
        QString path = QString("%1%2").arg(d->root).arg(dir.dirName);
        for (auto &name: dir.files) {
            cb(path + name);
        }
    }
}

void GitFile::visitPath(const GitFile::DirCallBack &cb) const
{
    for (auto &dir: d->dirs) {
        QString path = QString("%1%2").arg(d->root).arg(dir.dirName);
        cb(path);
    }
}
