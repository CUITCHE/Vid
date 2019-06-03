#define QT_MESSAGELOGCONTEXT

#include "FileMonitor.h"
#include <QFileSystemWatcher>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QMap>

struct FileInfo {
    QString filepath;
    QDateTime lastModificationTime;
};

using FileInfoType = QMap<QString, FileInfo>;


FileMonitor::FileMonitor(QObject *parent)
: QObject(parent)
, watcher(new QFileSystemWatcher)
{
    connect(watcher, &QFileSystemWatcher::fileChanged, this, &FileMonitor::onFileChanged);
    connect(watcher, &QFileSystemWatcher::directoryChanged, this, &FileMonitor::onDirectoryChanged);
}

FileMonitor::~FileMonitor() {
    stop();
}

static void obtainNewFiles(const QDir &dir,
                           QFileInfoList &maybayNew,
                           const QStringList &nameFilters,
                           const QDateTime &matchDate,
                           const QStringList &exitsFiles) {
    QFileInfoList fileInfoList = dir.entryInfoList(nameFilters, QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs);
    for (auto fileInfo: fileInfoList) {
        if (fileInfo.isDir()) {
            auto dir = QDir(fileInfo.filePath());
            obtainNewFiles(dir, maybayNew, nameFilters, matchDate, exitsFiles);
        } else {
            if (fileInfo.fileTime(QFileDevice::FileModificationTime).msecsTo(matchDate) < 1000) { // 去除系统影响，获取1s内改动的文件
                maybayNew << fileInfo;
            } else if (!exitsFiles.contains(fileInfo.filePath())) {
                // 虽然不是最近修改，但是从其它地方拷贝过来，也算新增
                maybayNew << fileInfo;
            }
        }
    }
}


void FileMonitor::start(const QString &path) {
    QFileInfo file(path);
    if (file.isDir() == false) {
        qCritical() << file.absolutePath() << ": is not a directory.";
        exit(-1);
    }
    if (file.exists() == false) {
        qCritical() << path << ": is not exists.";
        exit(-1);
    }
    if (file.isReadable() == false) {
        qCritical() << path << ": no permission to read.";
        exit(-1);
    }
    watcher->addPath(path);
    QDir dir(path);
    QStringList nameFilters;
    nameFilters << "*.swift" << "*.h" << "*.cpp" << "*.c" << "*.java" << "*.xml" << "*.hpp";
    obtainAllFile(dir, existsFilePath, nameFilters);
    qDebug() << "已存在文件数量：" << existsFilePath.count();
    qDebug() << existsFilePath;
}


void FileMonitor::stop() {
    watcher->removePaths(watcher->files());
    watcher->removePaths(watcher->directories());
    existsFilePath.clear();
}

void FileMonitor::obtainAllFile(const QDir &dir, QStringList &all, const QStringList &nameFilters)
{
    QFileInfoList fileInfoList = dir.entryInfoList(nameFilters, QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs);
    for (auto fileInfo: fileInfoList) {
        if (fileInfo.isDir()) {
            auto dir = QDir(fileInfo.filePath());
            obtainAllFile(dir, all, nameFilters);
        } else {
            all << fileInfo.filePath();
        }
    }
}

void FileMonitor::onFileChanged(const QString &path) {
    qDebug() << "文件改变";
    QFile f(path);
    if (f.exists()) {
        emit fileChanged(path, f.exists() ? modified : removed);
        qDebug() << __func__ << path << ": modified";
    } else {
        existsFilePath.removeAll(path);
    }
}

static void _remove_not_exists_file(QStringList &existsFilePath, FileMonitor *fm) {
    auto begin = existsFilePath.begin();
    while (begin != existsFilePath.end()) {
        QFile f(*begin);
        if (!f.exists()) {
            qDebug() << __func__ << *begin << ": removed";
            emit fm->fileChanged(*begin, FileMonitor::removed);
            begin = existsFilePath.erase(begin);
        } else {
            ++begin;
        }
    }
}

void FileMonitor::onDirectoryChanged(const QString &path) {
    qDebug() << "目录改变: " << path;
    QDir dir(path);
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QStringList nameFilters;
    nameFilters << "*.swift" << "*.h" << "*.cpp" << "*.c" << "*.java" << "*.xml" << "*.hpp";
    QFileInfoList changedFiles;
    obtainNewFiles(dir, changedFiles, nameFilters, QDateTime::currentDateTime(), existsFilePath);
    qDebug() << "changed files: " << changedFiles;
    if (changedFiles.isEmpty()) {
        _remove_not_exists_file(existsFilePath, this);
    } else {
        auto files = watcher->files();
        for (auto file : changedFiles) {
            auto path = file.filePath();
            if (!files.contains(path)) {
                watcher->addPath(path);
                existsFilePath.append(path);
                emit fileChanged(path, FileChangeType::add);
                qDebug() << path << "新增";
            }
        }
    }
}
