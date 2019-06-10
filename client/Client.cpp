#include "Client.h"
#include <QTcpSocket>
#include <QHostAddress>
#include "model/requests.pb.h"
#include "model/base.pb.h"
#include "model/SerializedTool.h"
#include "model/responses.pb.h"
#include "FileMonitor.h"
#include <QFile>


Client::Client(QObject *parent)
    : QObject(parent)
    , LoggerI(parent)
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
            logger->i("{}\t{}.", this->sock->errorString(), e);
        }
    });
}

void Client::connectToHost(const QString &host, uint16_t port)
{
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
        auto &res = *static_cast<const communication::Response *>(ptr);
        if (res.id() != queryId) {
            logger->warning("请求id不匹配");
            return ;
        }
        logger->i("登录：code: {}, msg: {}。开始验证目录", res.code(), res.msg());
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
        auto &res = *static_cast<const communication::Response *>(ptr);
        if (res.id() != queryId) {
            logger->warning("请求id不匹配");
            return;
        }
        if (res.code() != 0) {
            logger->w("文件校验失败：{}.", res.msg());
            return ;
        }
        logger->info("diff功能已就绪，现在开始愉快的写代码吧！");
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
            logger->i("{}：文件大于10M，请手动传输", file->fileName());
            return;
        }
        QByteArray content = file->readAll();
        body.set_content(content.toStdString());
    }

    int queryId = _get_query_id();
    requestReact.insert(queryId, [queryId, this](const void *ptr) {
        auto &res = *static_cast<const communication::Response *>(ptr);
        if (res.id() != queryId) {
            this->logger->warning("请求id不匹配");
            return;
        }
        qWarning() << "file diff：" << res.msg().c_str();
    });

    communication::Request req;
    req.set_id(queryId);
    req.set_proto(communication::Protocol::file_diff);
    req.set_body(body.SerializeAsString());
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

    communication::Response res;
    if (res.ParseFromString(buffer.toStdString()) == false) {
        logger->fatal("数据格式错误");
        exit(-1);
    }
    auto iter = requestReact.find(res.id());
    if (iter != requestReact.end()) {
        (*iter)(&res);
    } else {
        logger->f("错误的响应id：{}", res.id());
        exit(31);
    }
}

int32_t Client::_get_query_id()
{
    return ++queryId;
}

