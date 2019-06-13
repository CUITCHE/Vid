#ifndef LOGGERI_H
#define LOGGERI_H

#include "model/Logger.h"

template<class T>
class LoggerI
{
public:
    explicit LoggerI(QObject *parent = nullptr)
    : logger(Logger::logger(typeid(T).name(), parent))
    {}
protected:
    Logger *logger;
};

#endif // LOGGERI_H
