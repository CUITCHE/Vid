#ifndef CONTROLCLIENT_H
#define CONTROLCLIENT_H

#include <QThread>
#include "client/FileMonitor.h"
#include "model/LoggerI.h"

class Client;
class FileMonitor;

class ControlClient : public QThread, public LoggerI<ControlClient>
{
    Q_OBJECT
public:
    explicit ControlClient(QObject *parent = nullptr);
    ~ControlClient() override;

    void start(const QString &watchPath, const QString &host, uint16_t port, bool strict);
    void setLoginInfo(const QString &name, const QString &token);
protected:
    void run() override;
signals:
    void diff(const QString &relativePath, const QString &absolutePath, qint32 status);
public slots:
    void onFileChanged(const QString &path, FileMonitor::FileChangeType type);
    void onDiffComplete(int32_t queryId, bool success);
    void stop(); // stop thread safely.
private:
    Client *client;
    FileMonitor *fm;
    struct ControlClientPrivate *data;
};

#endif // CONTROLCLIENT_H
