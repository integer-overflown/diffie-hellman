#ifndef LAB4_DIFFIE_HELLMAN_LOGGING_H
#define LAB4_DIFFIE_HELLMAN_LOGGING_H

#include <QLoggingCategory>

namespace lab4::message::logging {
Q_LOGGING_CATEGORY(serialization, "message.serialization")
Q_LOGGING_CATEGORY(trace, "message.trace", QtCriticalMsg)
}

#endif // LAB4_DIFFIE_HELLMAN_LOGGING_H
