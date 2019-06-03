#include "SerializedTool.h"
#include <QByteArray>
#include <QtEndian>

namespace SerializedTool {
   union INT {
       char mem[4];
       int32_t value;
   };

    QByteArray intToChar(int32_t value) {
        INT f;
        f.value = qToBigEndian<int32_t>(value);
        return QByteArray(f.mem, 4);
    }

    int32_t charToInt(QByteArray data) {
        Q_ASSERT(data.count() == 4);
        INT f;
        memcpy(f.mem, data.data(), 4);
        return  qFromBigEndian<int32_t>(f.value);
    }

}
