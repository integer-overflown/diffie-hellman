#include "message.h"

#include <QJsonDocument>
#include <QLoggingCategory>

#include "parsing_utils.h"

namespace lab4::message {

namespace logging {
Q_LOGGING_CATEGORY(serialization, "message.serialization")
Q_LOGGING_CATEGORY(trace, "message.trace", QtCriticalMsg)
}

namespace {
auto
body(const QString& messageType,
     std::initializer_list<QPair<QString, QJsonValue>> args)
{
  QJsonObject obj(args);
  obj["type"] = messageType;
  return obj;
}
}

serialization::DeserializeResult<Hello>
Hello::fromJsonObject(const QJsonObject& object)
{
  Hello value;

  PARSE_REQUIRED_FIELD_OR_ELSE_ERROR(value.selfId, object, "selfId", String);

  return value;
}

serialization::SerializeResult
Hello::toJsonObject() const
{
  return body("HELLO", { { "selfId", selfId } });
}

serialization::DeserializeResult<CryptoSetup>
CryptoSetup::fromJsonObject(const QJsonObject& object)
{
  QString g;
  QString n;

  namespace mp = boost::multiprecision;

  PARSE_REQUIRED_FIELD_OR_ELSE_ERROR(g, object, "g", String);
  PARSE_REQUIRED_FIELD_OR_ELSE_ERROR(n, object, "n", String);

  // TODO: need to check the behavior for invalid strings
  return CryptoSetup{
    .g = mp::cpp_int{ g.toUtf8().constData() },
    .n = mp::cpp_int{ n.toUtf8().constData() },
  };
}

serialization::SerializeResult
CryptoSetup::toJsonObject() const
{
  return body("CRYPTO_SETUP",
              { { "g", QString::fromStdString(to_string(g)) },
                { "n", QString::fromStdString(to_string(n)) } });
}

}
