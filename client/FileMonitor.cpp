#define QT_MESSAGELOGCONTEXT

#include "FileMonitor.h"
#include <QFileSystemWatcher>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QMap>
#include <QSet>

struct FileInfo {
    QString filepath;
    QDateTime lastModificationTime;
};

struct FileMonitorPrivate {
    QFileSystemWatcher *watcher = new QFileSystemWatcher;
    QStringList existsFilePath; // 最开始监听时的文件目录情况
};

using FileInfoType = QMap<QString, FileInfo>;

#define CASE(type) \
    case type: return #type

const char *FileMonitor::translate(FileMonitor::FileChangeType type)
{
    switch (type) {
        CASE(modified);
        CASE(add);
        CASE(removed);
    }
    return "";
}

FileMonitor::FileMonitor(QObject *parent)
    : QObject(parent)
    , LoggerI(parent)
    , data(new FileMonitorPrivate)
{
    connect(data->watcher, &QFileSystemWatcher::directoryChanged, this, &FileMonitor::onDirectoryChanged);
}

FileMonitor::~FileMonitor() {
    stop();
    delete data;
}

void FileMonitor::start(const QString &path) {
    QFileInfo file(path);
    if (file.isAbsolute() == false) {
        logger->f("{} is not a absolute path.", path);
        exit(-1);
    }
    if (file.isDir() == false) {
        logger->f("'{}' is not a directory.", file.absolutePath());
        exit(-1);
    }
    if (file.exists() == false) {
        logger->f("'{}' is not exists.", path);
        exit(-1);
    }
    if (file.isReadable() == false) {
        logger->f("{} no permission to read.", path);
        exit(-1);
    }
    data->watcher->addPath(path);
    QDir dir(path);
    QStringList nameFilters;
    QStringList allDirectories;
    nameFilters << "*.swift" << "*.h" << "*.cpp" << "*.c" << "*.java" << "*.xml" << "*.hpp";
    obtainAllFile(dir, data->existsFilePath, &allDirectories, nameFilters);
    if (allDirectories.isEmpty() == false) {
        data->watcher->addPaths(allDirectories); // 只监听监听每一级的目录
    }
    logger->i("已存在文件数量：{}. => {}", data->existsFilePath.count(), data->existsFilePath.join(", "));
}


void FileMonitor::stop() {
    auto watcher = data->watcher;
    if (watcher->files().isEmpty() == false) {
        watcher->removePaths(watcher->files());
    }
    if (watcher->directories().isEmpty() == false) {
        watcher->removePaths(watcher->directories());
    }

    data->existsFilePath.clear();
}

void FileMonitor::obtainAllFile(const QDir &dir, QStringList &allFiles, QStringList *allDirectories, const QStringList &nameFilters)
{
    QFileInfoList fileInfoList = dir.entryInfoList(nameFilters, QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs);
    for (auto fileInfo: fileInfoList) {
        if (fileInfo.isDir()) {
            auto dir = QDir(fileInfo.filePath());
            allDirectories ? allDirectories->append(dir.path()) : qt_noop();
            obtainAllFile(dir, allFiles, allDirectories, nameFilters);
        } else {
            allFiles << fileInfo.filePath();
        }
    }
}

static void _remove_not_exists_file(QStringList &existsFilePath, FileMonitor *fm) {
    auto begin = existsFilePath.begin();
    while (begin != existsFilePath.end()) {
        QFile f(*begin);
        if (!f.exists()) {
            emit fm->fileChanged(*begin, FileMonitor::removed);
            begin = existsFilePath.erase(begin);
        } else {
            ++begin;
        }
    }
}

static void on_directory_changed(QFileInfoList changedFiles, FileMonitorPrivate *data, FileMonitor *self) {
    auto watcher = data->watcher;
    for (auto file : changedFiles) {
        auto path = file.filePath();
        if (file.isDir()) {
            if (!watcher->directories().contains(path)) {
                watcher->addPath(path);
            }
            continue;
        }
        if (data->existsFilePath.contains(path)) {
            emit self->fileChanged(path, FileMonitor::modified);
        } else {
            data->existsFilePath.append(path);
            emit self->fileChanged(path, FileMonitor::add);
        }
    }
}

static QString tostring(QFileInfoList f) {
    QStringList desc;
    for (auto &file: f) {
        desc.append(file.filePath());
    }
    return desc.join("\t");
}

void FileMonitor::onDirectoryChanged(const QString &path) {
    auto currentDateTime = QDateTime::currentDateTime();
    QDir dir(path);
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QStringList nameFilters;
    nameFilters << "*.swift" << "*.h" << "*.cpp" << "*.c" << "*.java" << "*.xml" << "*.hpp";
    QFileInfoList changedFiles;
    obtainNewFiles(dir, changedFiles, nameFilters, currentDateTime);
    logger->d("path={} has changed.", path);
    logger->d("changed files: {}", tostring(changedFiles));
    if (changedFiles.isEmpty()) {
        _remove_not_exists_file(data->existsFilePath, this);
    } else {
        on_directory_changed(changedFiles, data, this);
    }
}

void FileMonitor::obtainNewFiles(const QDir &dir, QFileInfoList &maybayNew, const QStringList &nameFilters, const QDateTime &matchDate)
{
    static constexpr auto TIME_DIFF = 1000;
    QFileInfoList fileInfoList = dir.entryInfoList(nameFilters, QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs);
    for (auto fileInfo: fileInfoList) {
        if (fileInfo.isDir()) {
            auto dir = QDir(fileInfo.filePath());
            if (fileInfo.fileTime(QFileDevice::FileModificationTime).msecsTo(matchDate) < TIME_DIFF) { // 新添加的目录
                maybayNew << fileInfo;
            }
        } else {
            auto time1 = matchDate.toMSecsSinceEpoch();
            auto time2 = fileInfo.fileTime(QFileDevice::FileModificationTime).toMSecsSinceEpoch();
            auto diff = time1 - time2;
            logger->d("filepath={}, currentTime={}, time={}, diff={}", fileInfo.filePath(), time1, time2, diff);
            if (fileInfo.fileTime(QFileDevice::FileModificationTime).msecsTo(matchDate) < TIME_DIFF) { // 去除系统影响，获取100ms内改动的文件
                maybayNew << fileInfo;
            } else if (!data->existsFilePath.contains(fileInfo.filePath())) {
                // 虽然不是最近修改，但是从其它地方拷贝过来，也算新增
                maybayNew << fileInfo;
            }
        }
    }
}
