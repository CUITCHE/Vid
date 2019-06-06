#include "model/Logger.h"
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QTextStream>
#include <QCoreApplication>
#include <QSemaphore>

static QSemaphore lock(1);

static QFile* _get_file() {
    static QFile *file;
    if (file) {
        return file;
    }
    auto fileName = QCoreApplication::applicationName() + ".log";
    file = new QFile(fileName);
    if (file->open(QIODevice::Append | QIODevice::Text) == false) {
        qDebug() << "can't open file: " << fileName;
        exit(-1);
    }
    return file;
}

static QTextStream& out() {
    static QTextStream *ts;
    if (ts) {
        return *ts;
    }
    ts = new QTextStream(_get_file());
    ts->setCodec("UTF-8");
    return *ts;
}

Logger::Logger(const QString &name, QObject *parent)
    : QObject (parent)
    , identifier(name)
{
}

Logger* Logger::logger(const QString &name, QObject *parent)
{
    auto _logger = new Logger(name, parent);
    return _logger;
}

Logger::~Logger()
{

}

void Logger::info(const QString &msg)
{
    write(msg, INFO);
}

void Logger::debug(const QString &msg)
{
    write(msg, DEBUG);
}

void Logger::warning(const QString &msg)
{
    write(msg, WARNING);
}

void Logger::error(const QString &msg)
{
    write(msg, ERROR);
}

void Logger::fatal(const QString &msg)
{
    write(msg, FATAL);
}

#define LEVEL_FMT_GEN(n) case Logger::n: return #n

static QString _string_level(Logger::Level l) {
    switch (l) {
        LEVEL_FMT_GEN(INFO);
        LEVEL_FMT_GEN(DEBUG);
        LEVEL_FMT_GEN(WARNING);
        LEVEL_FMT_GEN(ERROR);
        LEVEL_FMT_GEN(FATAL);
    }
    return "UNKNOWN";
}

static void _string_format_append(QString &msg, Logger::Level l) {
    switch (l) {
    case Logger::INFO:
    case Logger::DEBUG:
        break;
    case Logger::WARNING:
        msg.prepend("\033[1;33m");
        msg.append("\033[0m");
        break;
    case Logger::ERROR:
        msg.prepend("\033[1;31m");
        msg.append("\033[0m");
        break;
    case Logger::FATAL:
        msg.prepend("\033[34m");
        msg.append("\033[0m");
        break;
    }
}


void Logger::write(const QString &content, Level level)
{
    QString text = _string_level(level);
    text.prepend("[");
    text.append(" ").append(identifier).append(" ");
    text.append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")).append("] ").append(content);
    _string_format_append(text, level);
    lock.acquire();
    endl(out() << text);
    lock.release();
}

void Logger::_write_fmt_impl(QString &content, const bool &value)
{
    auto idx = content.indexOf("{}");
    if (idx == -1) {
        qWarning("No match placeholder for message: '%s' for value = %s", content.toStdString().c_str(), value ? "true" : "false");
        return;
    }
    content.replace(idx, 2, value ? "true" : "false");
}

void Logger::_write_fmt_impl(QString &content, const QString &value)
{
    auto idx = content.indexOf("{}");
        if (idx == -1) {
            qWarning("No match placeholder for message: '%s' for value = %s", content.toStdString().c_str(), value.toStdString().c_str());
            return;
        }
        content.replace(idx, 2, value);
}

void Logger::_write_fmt_impl(QString &content, const char *value)
{
    auto idx = content.indexOf("{}");
    if (idx == -1) {
        qWarning("No match placeholder for message: '%s' for value = %s", content.toStdString().c_str(), value);
        return;
    }
    content.replace(idx, 2, QString(value));
}

void Logger::_write_fmt_impl(QString &content, const std::string &value)
{
    _write_fmt_impl(content, QString::fromStdString(value));
}
