#ifndef CONTROLCLIENT_H
#define CONTROLCLIENT_H

#include <QObject>
#include "client/FileMonitor.h"

class Client;
class FileMonitor;

class ControlClient : public QObject
{
    Q_OBJECT
public:
    explicit ControlClient(QObject *parent = nullptr);

    void start(const QString &watchPath, const QString &host, uint16_t port);
    void setLoginInfo(const QString &name, const QString &token);
signals:

public slots:
    void onFileChanged(const QString &path, FileMonitor::FileChangeType type);
private:
    Client *client;
    FileMonitor *fm;
    QString watchPath;
    QString name, token;
};

#endif // CONTROLCLIENT_H
