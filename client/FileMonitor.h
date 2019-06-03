#ifndef FILEMONITOR_H
#define FILEMONITOR_H

#include <QObject>

class FileMonitorPrivate;
class QFileSystemWatcher;
class QDir;


class FileMonitor : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileMonitor)
public:
    enum FileChangeType {
        modified = 1, // 文件内容发生改变
        add = 2, // 新增文件
        removed = 3 // 移除文件
    };
public:
    explicit FileMonitor(QObject *parent = nullptr);
    ~FileMonitor();

    void start(const QString &path); // need directory
    void stop();

    static void obtainAllFile(const QDir &dir, QStringList &all, const QStringList &nameFilters);
signals:
    void fileChanged(const QString &path, FileChangeType type);
public slots:
    void onFileChanged(const QString &path);
    void onDirectoryChanged(const QString &path);
private:
    QFileSystemWatcher *watcher;
    QStringList existsFilePath; // 最开始监听时的文件目录情况

};

#endif // FILEMONITOR_H
