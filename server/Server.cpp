#include "Server.h"
#include <QTcpServer>
#include <QTcpSocket>
#include "model/requests.pb.h"
#include "model/responses.pb.h"
#include "model/base.pb.h"
#include "model/SerializedTool.h"
#include "client/FileMonitor.h"
#include <QTime>
#include <QTimer>
#include <QDir>
#include <QSet>

static QString random_string_8() {
    std::string tmp("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    qsrand(static_cast<uint>(QTime(0, 0, 0).secsTo(QTime::currentTime())));
    std::sort(tmp.begin(), tmp.end(),  [](const char, const char) {
        return qrand() % 47 > 23;
    });
#ifndef PRODUCT
    return "";
#else
    return QString::fromStdString(tmp).left(8);
#endif
}

Server::Server(QObject *parent)
    : QObject(parent)
    , LoggerI(parent)
    , server(new QTcpServer(this))
{
    connect(server, &QTcpServer::newConnection, this, &Server::onNewConnection);
    router.insert(communication::Protocol::token_login, &Server::onTokenLogin);
    router.insert(communication::Protocol::directory_verification, &Server::onDirectoryVerification);
    router.insert(communication::Protocol::file_diff, &Server::onFileDiff);
}

void Server::startListen(quint16 port, const QString &watchPath)
{
    this->watchPath = watchPath;
    auto b = server->listen(QHostAddress::Any, port);
    if (!b) {
        qWarning() << server->errorString();
    }
    passwd = random_string_8();
    logger->i("listen on {}. Token: {}.", port, passwd);

}

void Server::onNewConnection()
{
    if (client) {
        logger->w("已接入一个客户端: {} {}。本服务同时只接受一个连接!", client->peerAddress(), client->peerName());
        return;
    }
    isLogin = false;
    client = server->nextPendingConnection();
    logger->i("新连接接入: {} {}.", client->peerAddress(), client->peerName());
    connect(client, &QTcpSocket::readyRead, this, &Server::onClientReadyRead);
    connect(client, &QTcpSocket::disconnected, this, &Server::onClientDisconnected);
    QTimer::singleShot(1000, this, [this]() {
        if (this->isLogin == false) {
            this->client->abort();
            this->client = nullptr;
        }
    });
}

void Server::onClientReadyRead()
{
    QByteArray lengthBuffer = client->read(4);
    int32_t length = SerializedTool::charToInt(lengthBuffer);
    QByteArray buffer = client->read(length);
    if (buffer.count() != length) {
        client->abort();
        client = nullptr;
        return;
    }

    communication::Request req;
    if (req.ParseFromArray(buffer.data(), buffer.length()) == false) {
        logger->e("{}: 请求数据格式错误", client->peerAddress());
        client->abort();
        client = nullptr;
    }
    auto iter = router.find(req.proto());
    if (iter != router.end()) {
        (this->*iter.value())(req, client);
    }
}

void Server::onClientDisconnected()
{
    logger->i("{}: 连接断开", client->peerAddress());
    client = nullptr;
}

void Server::sendResponse(const communication::Response &res, QTcpSocket *sock)
{
    QByteArray data = QByteArray::fromStdString(res.SerializeAsString());
    QByteArray length = SerializedTool::intToChar(data.count());
    data.prepend(length);
    sock->write(data);
}

void Server::onTokenLogin(const communication::Request &req, QTcpSocket *sock)
{
    communication::Response res;
    res.set_id(req.id());
    communication::request::TokenLogin tokenLogin;
    do {
        if (tokenLogin.ParseFromString(req.body()) == true) {
            if (tokenLogin.token() == passwd.toStdString()) {
                res.set_code(0);
                res.set_msg("success");
                logger->i("Welcome: {}", tokenLogin.name());
                isLogin = true;
                break;
            }
        }
        isLogin = false;
        res.set_code(403);
        res.set_msg("登入口令或数据包格式错误");
    } while (0);

    sendResponse(res, sock);
}

extern QMap<QString, QString> files_hahs(QStringList paths);

void Server::onDirectoryVerification(const communication::Request &req, QTcpSocket *sock)
{
    communication::Response res;
    res.set_id(req.id());

    communication::request::DirectoryVerification dv;
    do {
        if (dv.ParseFromString(req.body()) == false) {
            res.set_code(403);
            res.set_msg("数据包格式不对");
            break;
        }
        auto curDir = QDir(watchPath);
        QString dirName = curDir.dirName();
        if (dirName.toStdString() != dv.root_name()) {
            res.set_code(403);
            res.set_msg("监控的根目录名不一致");
            break;
        }
        QStringList all; // TODO: 改为相对路径，程序可在任意目录下监控任意目录
        QStringList nameFilters;
        nameFilters << "*.swift" << "*.h" << "*.cpp" << "*.c" << "*.java" << "*.xml" << "*.hpp";
        FileMonitor::obtainAllFile(curDir, all, nameFilters);

        for (int i=0; i<all.size(); ++i) {
            all[0] = all[0].mid(watchPath.length() + 1);
        }
        const auto &contents = dv.contents();

        auto begin = contents.begin();
        QStringList clientFiles;
        while (begin != contents.end()) {
            clientFiles << QString::fromStdString(begin->first);
            ++begin;
        };
        QSet<QString> server = QSet<QString>::fromList(all);
        QSet<QString> client = QSet<QString>::fromList(clientFiles);
        auto intersection = server.intersect(client);
        QString msg;
        for (auto &path : server.subtract(intersection)) {
            msg.append(path).append(": 远端存在的文件，本端不存在\n");
        }
        for (auto &path : client.subtract(intersection)) {
            msg.append(path).append(": 本端存在的文件，远端不存在\n");
        }
        if (msg.length() != 0) {
            res.set_code(4);
            res.set_msg(msg.toStdString());
        }

        auto hash_files = files_hahs(all);
        auto iter = hash_files.begin();
        QStringList differences;
        while (iter != hash_files.end()) {
            auto client_iter = contents.find(iter.key().toStdString());
            if (iter.value().toStdString() != client_iter->second) {
                QString msg = iter.value();
                msg.append(": 两端文件hash值不一致\n");
                differences << msg;
            }
            ++iter;
        };
        res.set_code(0);
        res.set_msg("success");
        communication::responses::DirectoryVerification body;
        body.set_root_name(dv.root_name());
        for (int i=0; i<differences.count(); ++i) {
            body.set_differents(i, differences[i].toStdString());
        }
        res.set_body(body.SerializeAsString());
    } while (0);

    sendResponse(res, sock);
}

#include <QDataStream>

void Server::onFileDiff(const communication::Request &req, QTcpSocket *sock)
{
    communication::Response res;
    res.set_id(req.id());
    do {
        communication::request::FileDiff fdiff;
        if (fdiff.ParseFromString(req.body()) == false) {
            res.set_code(403);
            res.set_msg("数据包格式不对");
            break;
        }
        auto path = watchPath;
        if (!path.endsWith("/")) {
            path.append("/");
        }
        path.append(fdiff.relative_path().c_str());
        QFile file(path);
        if (fdiff.status() == FileMonitor::removed) {
            if (file.exists() == false) {
                res.set_code(0);
                res.set_msg("success");
                break;
            }
            auto b = file.remove();
            res.set_code(b ? 0 : 5);
            res.set_msg(b ? "success": "remove failed");
            logger->i("{}: remove {}.", path, b ? "success" : "failed");
            break;
        } else if (fdiff.status() == FileMonitor::add) {
            auto parentDirPath = QDir(path.section("/", 0, -2));
            if (parentDirPath.exists() == false) { // 有可能出现上级目录不存在的情况
                auto b = parentDirPath.mkpath(parentDirPath.path());
                logger->i("{}: create directory {}.", parentDirPath.path(), b ? "success" : "failed");
            }
            if (file.open(QIODevice::WriteOnly | QIODevice::Text) == false) {
                res.set_code(403);
                auto msg = path;
                msg.append(": 无法在此处新建文件");
                res.set_msg(msg.toStdString());
                logger->info(msg);
                break;
            }
        } else if (fdiff.status() == FileMonitor::modified) {
            if (file.open(QIODevice::WriteOnly | QIODevice::Text) == false) {
                res.set_code(403);
                QString msg = path;
                msg.append(": 无法读取文件");
                res.set_msg(msg.toStdString());
                logger->info(msg);
                break;
            }
        }

        // 修改文件
        file.write(QByteArray::fromStdString(fdiff.content()));
        file.close();
        res.set_code(0);
        res.set_msg("success");
    } while(0);

    sendResponse(res, sock);
}
