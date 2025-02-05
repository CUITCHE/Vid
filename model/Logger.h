#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <string>


#define CONTAINS_FUNCTION(method_name) \
    template<typename T> \
    struct contain_##method_name { \
        template<typename U, QString(U::*)()const> struct Dummy; \
        template<typename U> static char test(Dummy<U, &U::method_name>*); \
        template<typename U> static int test(...); \
        constexpr static bool value = sizeof (test<T>(0)) == sizeof (char); \
    }

CONTAINS_FUNCTION(toString);

#if defined (LOGGING_DEBUG)
#define LOGGING_LEVEL 0
#elif defined (LOGGING_INFO)
#define LOGGING_LEVEL 1
#elif defined (LOGGING_WARNING)
#define LOGGING_LEVEL 2
#elif defined (LOGGING_ERROR)
#define LOGGING_LEVEL 3
#elif defined (LOGGING_FATAL)
#define LOGGING_LEVEL 4
#else
#define LOGGING_LEVEL 0
#endif

class Logger : public QObject
{
    Q_OBJECT
public:
    enum Level {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3,
        FATAL = 4
    };
    explicit Logger(const QString  &name, QObject *parent = nullptr);
    static Logger* logger(const QString &name, QObject *parent = nullptr);
    ~Logger();

#if LOGGING_LEVEL <= 0
    template<typename ...Ty>
    void d(const QString &format, Ty&&... values) {
        _write(format, DEBUG, std::forward<Ty>(values)...);
    }
#else
    void d(...) {}
#endif

#if LOGGING_LEVEL <= 1
    template<typename ...Ty>
    void i(const QString &format, Ty&&... values) {
        _write(format, INFO, std::forward<Ty>(values)...);
    }
#else
    void i(...) {}
#endif

#if LOGGING_LEVEL <= 2
    template<typename ...Ty>
    void w(const QString &format, Ty&&... values) {
        _write(format, WARNING, std::forward<Ty>(values)...);
    }
#else
    void w(...) {}
#endif


#if LOGGING_LEVEL <= 3
    template<typename ...Ty>
    void e(const QString &format, Ty&&... values) {
        _write(format, ERROR, std::forward<Ty>(values)...);
    }
#else
    void e(...) {}
#endif

#if LOGGING_LEVEL <= 4
    template<typename ...Ty>
    void f(const QString &format, Ty&&... values) {
        _write(format, FATAL, std::forward<Ty>(values)...);
    }
#else
    void f(...) {}
#endif

public slots:
    void info(const QString &msg);
    void debug(const QString &msg);
    void warning(const QString &msg);
    void error(const QString &msg);
    void fatal(const QString &msg);

    void write(const QString &content, Level level);
private:
    template<typename ...Ty>
    void _write(const QString &format, Level level, Ty&&... values);

    template<typename ...Ty, typename T>
    void _write_fmt(QString &content, const T &value, const Ty... values);

    void _write_fmt(QString &) { }

    void _write_fmt_impl(QString &content, const bool &value);

#define gen_write_fmt_impl(type) \
    void _write_fmt_impl(QString &content, const type &value) { \
        auto idx = content.indexOf("{}");\
        if (idx == -1) {\
            qWarning("No match placeholder for message: '%s' for value = %s", content.toStdString().c_str(), QString::number(value).toStdString().c_str());\
            return;\
        }\
        content.replace(idx, 2, QString::number(value));\
    }\

    gen_write_fmt_impl(qint8)
    gen_write_fmt_impl(quint8)
    gen_write_fmt_impl(qint16)
    gen_write_fmt_impl(quint16)
    gen_write_fmt_impl(qint32)
    gen_write_fmt_impl(quint32)
    gen_write_fmt_impl(qint64)
    gen_write_fmt_impl(quint64)
    gen_write_fmt_impl(ulong)
    gen_write_fmt_impl(long)
    gen_write_fmt_impl(double)
#undef gen_write_fmt_impl

    template<class T>
    typename std::enable_if<contain_toString<T>::value>::type
    _write_fmt_impl(QString &content, const T& value) { _write_fmt_impl(content, value.toString()); }

    void _write_fmt_impl(QString &content, const QString &value);

    void _write_fmt_impl(QString &content, const char *value);

    void _write_fmt_impl(QString &content, const std::string &value);

private:
    QString identifier;
};

template<typename ...Ty, typename T>
void Logger::_write_fmt(QString &content, const T &value, const Ty... values)
{
    _write_fmt_impl(content, value);
    _write_fmt(content, values...);
}

template<typename ...Ty>
void Logger::_write(const QString &format, Logger::Level level, Ty&&... values)
{
    QString content = format;
    _write_fmt(content, std::forward<Ty>(values)...);
    write(content, level);
}

#endif // LOGGER_H
