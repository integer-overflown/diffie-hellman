#include "logging.h"

#include "network.h"

namespace lab4::message::logging {
Q_LOGGING_CATEGORY(serialization, "message.serialization")
Q_LOGGING_CATEGORY(trace, "message.trace", QtCriticalMsg)
Q_LOGGING_CATEGORY(net, "message.net")
}
