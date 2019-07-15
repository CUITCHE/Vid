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
    QString file_path;  // 文件绝对路径
    bool sent = false; // 上次添加file-meta后是否已经发送信号
    qint64 last_modified_time: 63; // 最近修改时间，msecs Since Epoch.
    qint64 last_content_size;  // 最近一次记录的文件大小

    FileMeta(const QString &filePath) : file_path(filePath) {
        QFileInfo file(file_path);
        Q_ASSERT(file.exists());
        last_modified_time = file.fileTime(QFileDevice::FileModificationTime).toMSecsSinceEpoch();
        last_content_size = file.size();
    }

    qint32 status() {
        QFileInfo file(file_path);
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
    QFileSystemWatcher *watcher = new QFileSystemWatcher;
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
    data->git->all_file(&allDirectories, &data->existsFilePath);
    data->watcher->addPaths(allDirectories);

    if (allDirectories.isEmpty() == false) {
        data->watcher->addPaths(allDirectories); // 只监听每一级的目录
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

static QStringList _new_add_directories(const QString &path, const QStringList &directories) {
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

void FileMonitor::onDirectoryChanged(const QString &path) {
    logger->d("path={} has changed.", path);

    auto changedFiles = _new_add_directories(path, data->watcher->directories());
    if (!changedFiles.isEmpty()) {
        data->watcher->addPaths(changedFiles);
    }

    QDir dir(data->git->roorDirPath());
    QStringList flags;
    data->git->status([this, &dir](const QStringList& newf, const QStringList& modifiedf, const QStringList& deletedf) {
        Q_UNUSED(deletedf);
        auto store = {newf, modifiedf, deletedf};

        for (auto &s : store) {
            if (s.isEmpty()) continue;
            for (auto &path : s) {
                auto filePath = dir.filePath(path);
                auto iter = data->filemeta.find(filePath);
                if (iter == data->filemeta.end()) {
                    data->filemeta.emplace(filePath, filePath);
                } else {
                    iter->second.sent = false;
                }
            }

        }
    });

    for (auto &pair: data->filemeta) {
        auto &fm = pair.second;
        qint32 status = fm.status();
        defer({
                  fm.sent = true;
              });
        switch (status) {
        case FileMeta::modified:
            emit fileChanged(fm.file_path, FileMonitor::modified);
            break;
        case FileMeta::removed:
            emit fileChanged(fm.file_path, FileMonitor::removed);
            break;
        case FileMeta::nochanges:
            if (fm.sent == false) {// is added newly
                emit fileChanged(fm.file_path, FileMonitor::add);
            }
            break;
        default:
            logger->w("Unknown file status={} (1, 3 or 1000)", status);
        }
    }
}
