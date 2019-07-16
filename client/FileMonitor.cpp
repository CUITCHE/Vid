#define QT_MESSAGELOGCONTEXT

#include "FileMonitor.h"
#include <QFileSystemWatcher>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QMap>
#include <QSet>
#include "git/Git.h"
#include "defer.h"
#include <unordered_map>

using namespace std;

struct QStringHasher {
    size_t operator()(const QString &val) const {
        return qHash(val);
    }
};

struct FileMeta {
    enum {
        modified = 1,
        removed = 3,
        nochanges = 1000
    };
    QString filepath;  // 文件绝对路径
    bool sent = false; // 上次添加file-meta后是否已经发送信号
    qint64 last_modified_time: 63; // 最近修改时间，msecs Since Epoch.
    qint64 last_content_size;  // 最近一次记录的文件大小

    FileMeta(const QString &filePath) : filepath(filePath) {
        QFileInfo file(filepath);
        Q_ASSERT(file.exists());
        last_modified_time = file.fileTime(QFileDevice::FileModificationTime).toMSecsSinceEpoch();
        last_content_size = file.size();
    }

    qint32 status() {
        QFileInfo file(filepath);
        if (!file.exists()) {
            return removed;
        }
        auto content_size = file.size();
        auto cur_modified_time = file.fileTime(QFileDevice::FileModificationTime).toMSecsSinceEpoch();
        defer ({
                   last_content_size = content_size;
                   last_modified_time = cur_modified_time;
               });
        if (content_size != last_content_size) {
            return modified;
        }
        if (cur_modified_time != last_modified_time) {
            return modified;
        }
        return nochanges;
    }
};

struct FileMonitorPrivate {
    QFileSystemWatcher *watcher;
    QStringList existsFilePath; // 最开始监听时的文件目录情况
    unordered_map<QString, FileMeta, QStringHasher> filemeta;
    Git *git = nullptr;
};


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
    data->watcher = new QFileSystemWatcher(this);
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
        logger->f("'{}' no permission to read.", path);
        exit(-1);
    }
    data->watcher->addPath(path);
    data->git = new Git(path, this);

    QStringList allDirectories;
    data->git->allFiles(&allDirectories, &data->existsFilePath);

    // Warnings: MacOS系统支持无限量监控，其它系统未知。如果要支持其它系统这里以及与之相关的逻辑需要更改。
    data->watcher->addPaths(allDirectories);
    data->watcher->addPaths(data->existsFilePath);

    for (auto &filepath: data->existsFilePath) {
        data->filemeta.emplace(filepath, filepath);
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
    delete data->git;
    data->existsFilePath.clear();
}

static QStringList _new_added_directories(const QString &path, const QStringList &directories) {
    QDir dir(path);
    QFileInfoList fileInfoList = dir.entryInfoList(QStringList() << "*", QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs);
    QStringList contents;
    for (auto fileInfo: fileInfoList) {
        if (fileInfo.isDir() && !directories.contains(fileInfo.filePath())) {
            contents << fileInfo.filePath();
        }
    }
    return  contents;
}

static QStringList _all_file_inpath(const QString &path, const QStringList &files) {
    QDir dir(path);
    QFileInfoList fileInfoList = dir.entryInfoList(QStringList() << "*", QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QStringList contents;
    for (auto fileInfo: fileInfoList) {
        if (fileInfo.isFile() && !files.contains(fileInfo.filePath())) {
            contents << fileInfo.filePath();
        }
    }
    return  contents;
}

void FileMonitor::onDirectoryChanged(const QString &path) {
    logger->i("path={} has changed.", path);

    {
        // check
        QDir dir(path);
        if (dir.exists() == false) {
            data->watcher->removePath(path);
            return;
        }

        auto new_dirs = _new_added_directories(path, data->watcher->directories());
        if (!new_dirs.isEmpty()) {
            data->watcher->addPaths(new_dirs);
        }
    }

    auto new_files = _all_file_inpath(path, data->watcher->files());
    QStringList flags;
    auto iter = new_files.begin();
    while (iter != new_files.end()) {
        const QString &filepath = *iter;
        if (data->git->isIgnored(filepath) == false) {
            if (data->filemeta.find(filepath) == data->filemeta.end()) {
                data->watcher->addPath(filepath);
                data->filemeta.emplace(filepath, filepath);
                flags << filepath;
            }
            ++iter;
        } else {
            iter = new_files.erase(iter);
        }
    }

    for (auto &filepath: flags) {
        auto iter = data->filemeta.find(filepath);
        if (iter != data->filemeta.end()) {
            auto &fm = iter->second;
            _file_changed(fm);
        } else {
            logger->e("logic error. file={} should be exists.", filepath);
        }
    }
}

void FileMonitor::onFileChanged(const QString &filepath) {
    logger->i("file={} has changed.", filepath);
    auto iter = data->filemeta.find(filepath);
    if (iter != data->filemeta.end()) {
        auto &fm = iter->second;
        _file_changed(fm);
    } else {
        logger->e("logic error. file={} should be exists.", filepath);
    }
}

void FileMonitor::_file_changed(FileMeta &fm) {
    qint32 status = fm.status();
    defer({
        fm.sent = true;
    });
    switch (status) {
        case FileMeta::modified:
            emit fileChanged(fm.filepath, FileMonitor::modified);
            break;
        case FileMeta::removed:
            if (fm.sent == false) {
                emit fileChanged(fm.filepath, FileMonitor::removed);
            }
            break;
        case FileMeta::nochanges:
            if (fm.sent == false) {// added newly
                emit fileChanged(fm.filepath, FileMonitor::add);
            }
            break;
        default:
            logger->w("Unknown file status={} (1, 3 or 1000)", status);
    }
}
