#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QMap>
#include <functional>
#include <atomic>
#include "model/LoggerI.h"

namespace communication {
    class Request;
}

class QTcpSocket;
class QFile;


class Client : public QObject, public LoggerI<Client>
{
    Q_OBJECT
public:
    using FileName = QString;
    using FileMD5 = QString;
    explicit Client(QObject *parent = nullptr);
    void connectToHost(const QString &host, uint16_t port);
    void sendData(const QByteArray &data);
    void sendRequest(const communication::Request &req);

    void abort();

    void tokenLogin(const QString &token, const QString &name);
    void directoryVerification(const QString &rootPath, const QString &rootName, const QMap<FileName, FileMD5> &contents, bool strict);
signals:
    void connected();
    void disconnected();
    void loginSuccess();
    void shouldBegin(); // 登录验证流程完成，可以开始监控文件了。
    void diffComplete(int32_t queryId, bool success); // 一次diff完成
public slots:
    void onReadyRead();
    void fileDiff(const QString &relativePath, const QString &absolutePath, qint32 status);
protected:
    int32_t _get_query_id();
private:
    QTcpSocket *sock;
    using Reactor = std::function<void(const void *ptr)>;
    QMap<int32_t, Reactor> requestReact;
    std::atomic_int32_t queryId;
};

#endif // CLIENT_H
