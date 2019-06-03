#ifndef SERIALIZEDTOOL_H
#define SERIALIZEDTOOL_H

#include <QObject>

namespace SerializedTool {
    QByteArray intToChar(int32_t value);
    int32_t charToInt(QByteArray data);
}

#endif // SERIALIZEDTOOL_H
