#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QMap>

class QTcpServer;
class QTcpSocket;

namespace communication {
    class Responese;
    class Request;
}

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    void startListen(quint16 port, const QString &watchPath);
signals:

public slots:
    void onNewConnection();
    void onClientReadyRead();
    void onClientDisconnected();
protected:
    void sendResponse(const communication::Responese &res, QTcpSocket *sock);
    void onTokenLogin(const communication::Request &req, QTcpSocket *sock);
    void onDirectoryVerification(const communication::Request &req, QTcpSocket *sock);
    void onFileDiff(const communication::Request &req, QTcpSocket *sock);
private:
    QTcpServer *server;
    QTcpSocket *client = nullptr;
    bool isLogin = false;
    QString passwd;
    QString watchPath;
    QMap<int32_t, void(Server::*)(const communication::Request &, QTcpSocket *)> router;
};

#endif // SERVER_H
