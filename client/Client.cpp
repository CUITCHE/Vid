#include "Client.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include "model/requests.pb.h"
#include "model/base.pb.h"
#include "model/SerializedTool.h"
#include "model/responses.pb.h"
#include "FileMonitor.h"
#include <QFile>


Client::Client(QObject *parent)
: QObject(parent)
, sock(new QTcpSocket(this))
{
    connect(sock, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(sock, &QTcpSocket::connected, this, [=]() {
        emit this->connected();
    });
    connect(sock, &QTcpSocket::disconnected, this, [=]() {
        emit this->disconnected();
    });
    connect(sock, static_cast<void(QTcpSocket::*)(QTcpSocket::SocketError)>(&QTcpSocket::error),
            this, [this](QAbstractSocket::SocketError e) {
        if (this->sock) {
            qDebug() << this->sock->errorString() << "\t" << e;
        }
    });
}

void Client::connectToHost(const QString &host, uint16_t port)
{
    qDebug() << "host: " << host;
    if (host.count(".") == 3) {
        sock->connectToHost(QHostAddress(host), port);
    } else {
        sock->connectToHost(host, port);
    }
}

void Client::sendData(const QByteArray &data)
{
    sock->write(data);
}

void Client::sendRequest(const communication::Request &req)
{
    QByteArray data = QByteArray::fromStdString(req.SerializeAsString());
    QByteArray length = SerializedTool::intToChar(data.count());
    data.prepend(length);
    sendData(data);
}

void Client::abort()
{
    requestReact.clear();
    sock->abort();
    sock = nullptr;
}

void Client::tokenLogin(const QString &token, const QString &name)
{
    int queryId = _get_query_id();
    requestReact.insert(queryId, [=](const void *ptr) {
        auto &res = *static_cast<const communication::Responese *>(ptr);
        if (res.id() != queryId) {
            qWarning() << "请求id不匹配";
            return ;
        }
        qDebug("登录：code: %d, msg: %s。开始验证目录", res.code(), res.msg().c_str());
        emit this->loginSuccess();
    });
    communication::request::TokenLogin body;
    body.set_token(token.toStdString());
    body.set_name(name.toStdString());

    communication::Request req;
    req.set_id(queryId);
    req.set_proto(communication::Protocol::token_login);
    req.set_body(body.SerializeAsString());

    sendRequest(req);
}

void Client::directoryVerification(const QString &rootPath, const QString &rootName, const QMap<Client::FileName, Client::FileMD5> &contents)
{
    int queryId = _get_query_id();
    requestReact.insert(queryId, [=](const void *ptr) {
        auto &res = *static_cast<const communication::Responese *>(ptr);
        if (res.id() != queryId) {
            qWarning() << "请求id不匹配";
            return;
        }
        if (res.code() != 0) {
            qWarning() << "文件校验失败：" << res.msg().c_str();
            return ;
        }
        communication::responses::DirectoryVerification dv;
        if (dv.ParseFromString(res.body()) == false) {
            qWarning() << "数据格式错误";
            return;
        }
        if (dv.root_name() != rootName.toStdString()) {
            qWarning() << "双方根路径不匹配";
            return ;
        }
        if (dv.differents().empty() == false) {
            qWarning() << "目录：" << rootPath << " 存在差异文件，请先保证目录文件一致: ";
            return ;
        }
        qDebug() << "diff功能已就绪，现在开始愉快的写代码吧！";
        emit this->shouldBegin();
    });
    communication::request::DirectoryVerification body;
    body.set_root_name(rootName.toStdString());
    auto &map = *body.mutable_contents();
    auto iter = contents.begin();
    while (iter != contents.end()) { // 使用相对路径
        map[iter.key().mid(rootPath.length() + 1).toStdString()] = iter.value().toStdString();
        ++iter;
    }

    communication::Request req;
    req.set_id(queryId);
    req.set_proto(communication::Protocol::directory_verification);
    req.set_body(body.SerializeAsString());

    sendRequest(req);

}

void Client::fileDiff(const QString &relativePath, QFile *file, int32_t status)
{
    communication::request::FileDiff body;
    body.set_relative_path(relativePath.toStdString());
    body.set_status(status);
    if (status != FileMonitor::removed) {
        if (file->isOpen() == false) {
            file->open(QIODevice::ReadOnly | QIODevice::Text);
        }

        if (file->size() > 10 * 1024 * 1024) {
            qDebug() << "该文件大于10M，请手动传输" << file;
            return;
        }
        QByteArray content = file->readAll();
        file->close();
        body.set_content(content.toStdString());
    }

    int queryId = _get_query_id();
    requestReact.insert(queryId, [=](const void *ptr) {
        auto &res = *static_cast<const communication::Responese *>(ptr);
        if (res.id() != queryId) {
            qWarning() << "请求id不匹配";
            return;
        }
        qWarning() << "file diff：" << res.msg().c_str();
    });

    communication::Request req;
    req.set_id(queryId);
    req.set_proto(communication::Protocol::file_diff);
    req.set_body(body.SerializeAsString());
    qDebug() << "发送diff请求";
    sendRequest(req);
}

void Client::onReadyRead()
{
    QByteArray lengthBuffer = sock->read(4);
    int32_t length = SerializedTool::charToInt(lengthBuffer);
    QByteArray buffer = sock->read(length);
    if (buffer.count() != length) {
        sock->abort();
        sock = nullptr;
        return;
    }

    communication::Responese res;
    if (res.ParseFromString(buffer.toStdString()) == false) {
        qDebug() << "数据格式错误";
        exit(-1);
    }
    auto iter = requestReact.find(res.id());
    if (iter != requestReact.end()) {
        (*iter)(&res);
    } else {
        qDebug() << "错误的响应id: " << res.id();
    }
}

int32_t Client::_get_query_id()
{
    return ++queryId;
}

