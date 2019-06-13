#include <QCoreApplication>
#include <QtDebug>
#include "control/ControlClient.h"
#include "server/Server.h"
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStandardPaths>
#include <QHostInfo>
#include <QTextStream>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>


void start_client(const QString &host, const QString &path, uint16_t port);
void testLogin();
void _main();

#include <QDir>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("Vid");
    QCoreApplication::setApplicationVersion("1.0.0");
#ifdef PRODUCT
    _main();
#else
    testLogin();
#endif
    return a.exec();
}


void start_client(const QString &host, const QString &path, uint16_t port) {
    ControlClient *control = new ControlClient();
    auto name = QString("[%1]-%2").arg(QHostInfo::localHostName(), QStandardPaths::writableLocation(QStandardPaths::HomeLocation).section("/", -1));
    qDebug("Welcome, %s ! Please Enter THE TOKEN which server given: ", name.toStdString().c_str()) ;

    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= static_cast<ulong>(~ICANON);
    newt.c_lflag &= static_cast<ulong>(~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    QString token;
    int c = 0;
    fflush(stdin);
    while ((c = getchar()) != '\n') {
        token.append(QChar(static_cast<uchar>(c)));
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    control->setLoginInfo(name, token);
    control->start(path, host, port);
}


void _main() {
    QCommandLineParser parser;
    parser.setApplicationDescription("A tool which Monitors file from client to server.");
    parser.addPositionalArgument("module", "启动模块的名称代号。只接受'client'和'server'");
    parser.addPositionalArgument("path", "监控路径，请使用绝对路径");
    parser.addPositionalArgument("port", "监听的端口号，client和server应使用同一个端口");

    QCommandLineOption address(QStringList() << "a" << "address", "ip地址，server会忽略这个值。默认是localhost");
    parser.addOption(address);
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

    parser.process(QCoreApplication::arguments());
    auto args = parser.positionalArguments();
    if (args.length() < 3) {
        qDebug() << "参数不足，至少三个参数[module, path, port]";
        exit(-1);
    }
    auto module = args[0];
    auto path  = args[1];
    auto port = static_cast<uint16_t>(args[2].toUInt());
    qDebug() << args;
    if (module == "client") {
        QString host = "localhost";
        if (parser.isSet(address)) {
            host = args[3];
        }
        start_client(host, path, port);
    } else if (module == "server") {
        Server *server = new Server;
        server->startListen(port, path);
    } else {
        qDebug() << "未知 module name: " << module;
        qDebug() << "Accepted module names: [client, server]";
    }
}


void testLogin() {
    auto args = QCoreApplication::arguments();
    if (args.count() > 1 && args[1] == "client") {
        qDebug() << "client start";
        ControlClient *control = new ControlClient();
        control->setLoginInfo("hejunqiu", "");
        control->start("/tmp/test1/work", "localhost", 9010);
    } else {
        Server *server = new Server;
        server->startListen(9010, "/tmp/test2/work");
    }
}
