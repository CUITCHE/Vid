#ifndef FILEMONITOR_H
#define FILEMONITOR_H

#include <QObject>
#include "model/LoggerI.h"
#include <QFileInfoList>

class QDir;


class FileMonitor : public QObject, public LoggerI<FileMonitor>
{
    Q_OBJECT
public:
    enum FileChangeType {
        modified = 1, // 文件内容发生改变
        add = 2, // 新增文件
        removed = 3 // 移除文件
    };
    static const char *translate(FileChangeType type);
public:
    explicit FileMonitor(QObject *parent = nullptr);
    ~FileMonitor() override;

    void start(const QString &path); // need directory
    void stop();
protected:
    void _file_changed(struct FileMeta &fm);

signals:
    void fileChanged(const QString &path, FileChangeType type);
public slots:
    void onDirectoryChanged(const QString &path);
    void onFileChanged(const QString &path);
private:
    struct FileMonitorPrivate *data;

};

#endif // FILEMONITOR_H
