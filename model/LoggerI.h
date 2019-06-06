#ifndef LOGGERI_H
#define LOGGERI_H

#include "model/Logger.h"

class LoggerI
{
public:
    LoggerI(QObject *parent = nullptr);
protected:
    Logger *logger;
};

#endif // LOGGERI_H
