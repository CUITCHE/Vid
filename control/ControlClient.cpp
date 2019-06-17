#include "ControlClient.h"
#include "client/Client.h"
#include <QFile>
#include <QDir>
#include <QMap>
#include <QSemaphore>
#include <QCryptographicHash>
#include <atomic>
#include <QPair>


static const int32_t INITIALIZE = 0;
static const int32_t BUSYING = ~0;


struct FilePathPool {
    using Data = QPair<QString, FileMonitor::FileChangeType>;
private:
    QList<Data> data;
    QSemaphore lock;

public:
    FilePathPool()
        : lock(1)
    {}

    bool isEmpty() {
        const QSemaphoreReleaser guard(lock);
        return data.empty();
    }

    void insert(const QString &path, FileMonitor::FileChangeType type) {
        const QSemaphoreReleaser guard(lock);
        if (type == FileMonitor::removed) {

            auto iter = std::find_if(data.begin(), data.end(), [&path](const Data& a) {
                return a.first == path;
            });
            if (iter != data.end()) {
                data.erase(iter);
            }
        }
        data.push_back({path, type});
    }

    QString description() {
        const QSemaphoreReleaser guard(lock);
        QString desc;
        for (auto &pair: data) {
            desc += QString("%1: %2\n").arg(pair.first, pair.second);
        }
        return desc;
    }

    QPair<QString, FileMonitor::FileChangeType> take_one() {
        const QSemaphoreReleaser guard(lock);
        return data.takeFirst();
    }
};


struct ControlClientPrivate {
    QString watchPath;
    QString name;
    QString token;
    FilePathPool pool;
    std::atomic_int32_t queryId;
    std::atomic_bool isRunning;
    bool strict = false;
    char padding[10];
};


QMap<QString, QString> files_hahs(QStringList paths) {
    QMap<QString, QString> contents;
    for (auto &path : paths) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            QString hash(QCryptographicHash::hash(file.readAll(), QCryptographicHash::Sha256).toHex());
            contents.insert(path, hash);
            file.close();
        }
    }
    return contents;
}


ControlClient::ControlClient(QObject *parent)
    : QThread(parent)
    , LoggerI(parent)
    , client(new Client(this))
    , fm(new FileMonitor(this))
    , data(new ControlClientPrivate())
{
    connect(fm, &FileMonitor::fileChanged, this, &ControlClient::onFileChanged);
    connect(client, &Client::disconnected, this, [this]() {
        this->fm->stop();
        this->client->abort();
        this->client = nullptr;
    });
    connect(client, &Client::connected, this, [this]() {
        this->client->tokenLogin(this->data->token, this->data->name);
    });

    connect(client, &Client::loginSuccess, this, [this]() {
        QDir dir(this->data->watchPath);
        QStringList nameFilters;
        nameFilters << "*.swift" << "*.h" << "*.cpp" << "*.c" << "*.java" << "*.xml" << "*.hpp";
        QStringList existsFilePath;
        FileMonitor::obtainAllFile(dir, existsFilePath, nullptr, nameFilters);
        auto contents = files_hahs(existsFilePath);
        this->client->directoryVerification(this->data->watchPath, dir.dirName(), contents, data->strict);
    });
    connect(client, &Client::shouldBegin, this, [this]() {
        this->fm->start(this->data->watchPath);
    });
    connect(client, &Client::diffComplete, this, &ControlClient::onDiffComplete);
    connect(this, &ControlClient::diff, client, &Client::fileDiff);
}

ControlClient::~ControlClient()
{
    delete data;
}

void ControlClient::start(const QString &watchPath, const QString &host, uint16_t port, bool strict)
{
    data->watchPath = watchPath;
    data->strict = strict;
    client->connectToHost(host, port);
    QThread::start();
}

void ControlClient::setLoginInfo(const QString &name, const QString &token)
{
    data->name = name;
    data->token = token;
}

void ControlClient::run()
{
    data->isRunning = true;
    data->queryId = INITIALIZE;
    Q_FOREVER {
        if (data->isRunning == false) break;

        if (data->pool.isEmpty()) {
            msleep(10); continue;
        }
        if (data->queryId != INITIALIZE) {
            msleep(10); continue;
        }
        data->queryId = BUSYING;

        auto pair = data->pool.take_one();
        auto &path = pair.first;
        auto type = pair.second;
        QString relativePath = path.mid(data->watchPath.length() + 1);

        emit diff(relativePath, path, type);

        logger->i("正在同步: path={}, 操作={}", pair.first, FileMonitor::translate(pair.second));
        if (!data->pool.isEmpty()) {
            logger->i("队列中剩余：{}", data->pool.description());
        }
    }
    logger->info("control quit...");
}

void ControlClient::onFileChanged(const QString &path, FileMonitor::FileChangeType type)
{
    logger->d("队列[添加]，path={} operator={}.", path, FileMonitor::translate(type));
    data->pool.insert(path, type);
}

void ControlClient::onDiffComplete(int32_t queryId, bool)
{
    logger->i("同步完成, queryId={}", queryId);
    data->queryId = INITIALIZE;
}

void ControlClient::stop()
{
    data->isRunning = false;
    fm->stop();
    client->abort();
}
