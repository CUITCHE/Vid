#include "LoggerI.h"

LoggerI::LoggerI(QObject *parent)
    : logger(Logger::logger(typeid(this).name(), parent))
{

}
