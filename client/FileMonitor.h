#ifndef FILEMONITOR_H
#define FILEMONITOR_H

#include <QObject>
#include "model/LoggerI.h"

class QDir;


class FileMonitor : public QObject, public LoggerI
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

    static void obtainAllFile(const QDir &dir, QStringList &allFiles, QStringList *allDirectories, const QStringList &nameFilters);
signals:
    void fileChanged(const QString &path, FileChangeType type);
public slots:
    void onFileChanged(const QString &path);
    void onDirectoryChanged(const QString &path);
private:
    struct FileMonitorPrivate *data;

};

#endif // FILEMONITOR_H
