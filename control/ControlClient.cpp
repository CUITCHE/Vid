#include "ControlClient.h"
#include "client/Client.h"
#include <QFile>
#include <QDir>
#include <QMap>
#include <QCryptographicHash>


QMap<QString, QString> files_hahs(QStringList paths) {
    QMap<QString, QString> contents;
    for (auto &path : paths) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString hash(QCryptographicHash::hash(file.readAll(), QCryptographicHash::Sha256).toHex());
            contents.insert(path, hash);
            file.close();
        }
    }
    return contents;
}



ControlClient::ControlClient(QObject *parent)
    : QObject(parent)
    , client(new Client(this))
    , fm(new FileMonitor(this))
{
    connect(fm, &FileMonitor::fileChanged, this, &ControlClient::onFileChanged);
    connect(client, &Client::disconnected, this, [this]() {
        this->fm->stop();
        this->client->abort();
        this->client = nullptr;
    });
    connect(client, &Client::connected, this, [this]() {
        this->client->tokenLogin(this->token, this->name);
    });

    connect(client, &Client::loginSuccess, this, [this]() {
        QDir dir(this->watchPath);
        QStringList nameFilters;
        nameFilters << "*.swift" << "*.h" << "*.cpp" << "*.c" << "*.java" << "*.xml" << "*.hpp";
        QStringList existsFilePath;
        FileMonitor::obtainAllFile(dir, existsFilePath, nameFilters);
        auto contents = files_hahs(existsFilePath);
        this->client->directoryVerification(this->watchPath, dir.dirName(), contents);
    });
    connect(client, &Client::shouldBegin, this, [this]() {
        this->fm->start(this->watchPath);
    });
}

void ControlClient::start(const QString &watchPath, const QString &host, uint16_t port)
{
    this->watchPath = watchPath;
    client->connectToHost(host, port);
}

void ControlClient::setLoginInfo(const QString &name, const QString &token)
{
    this->name = name;
    this->token = token;
}


void ControlClient::onFileChanged(const QString &path, FileMonitor::FileChangeType type)
{
    QFile *file = nullptr;
    QString relativePath = path.mid(watchPath.length() + 1);
    if (type != FileMonitor::removed) {
        file = new QFile(path);
    }
    client->fileDiff(relativePath, file, type);
    if (file) {
        file->close();
        delete file;
    }
}
