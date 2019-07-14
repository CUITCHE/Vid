#include <QCoreApplication>
#include <QtDebug>
#include "control/ControlClient.h"
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStandardPaths>
#include <QHostInfo>
#include <QTextStream>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <git/Git.h>


void start_client(const QString &host, const QString &path, uint16_t port, bool strict);
void testLogin();
void _main();

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("Vid");
    QCoreApplication::setApplicationVersion("1.0.0");
    __go();
    exit(0);
#ifdef PRODUCT
    _main();
#else
    testLogin();
#endif
    return a.exec();
}


void start_client(const QString &host, const QString &path, uint16_t port, bool strict) {
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
    control->start(path, host, port, strict);
}


void _main() {
    QCommandLineParser parser;
    parser.setApplicationDescription("A tool which Monitors file from client to server.");
    parser.addPositionalArgument("host", "server地址");
    parser.addPositionalArgument("path", "监控路径，请使用绝对路径");
    parser.addPositionalArgument("port", "监听的端口号，client和server应使用同一个端口");

    parser.addOption(QCommandLineOption(QStringList() << "s" << "strict", "是否启用严格模式，在此模式下，会忽略启动时对两端文件的check", "strict", "false"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

    parser.process(QCoreApplication::arguments());
    auto args = parser.positionalArguments();
    if (args.length() < 3) {
        qDebug() << "固定参数不足，需要3个参数[module, path, port]";
        exit(-1);
    }
    auto host = args[0];
    auto path  = args[1];
    auto port = static_cast<uint16_t>(args[2].toUInt());
    auto strict = parser.value("strict").toLower() == "true" ? true : false;
    qDebug() << args;
    start_client(host, path, port, strict);
}


void testLogin() {
    qDebug() << "client start";
    ControlClient *control = new ControlClient();
    control->setLoginInfo("hejunqiu", "");
    control->start("/tmp/test1/work", "localhost", 9010, false);}
