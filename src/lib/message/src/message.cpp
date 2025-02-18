#include "message.h"

#include <QJsonDocument>
#include <QLoggingCategory>

#include "logging.h"
#include "parsing_utils.h"

namespace mp = boost::multiprecision;

namespace lab4::message {

namespace {
auto
body(const QString& messageType,
     std::initializer_list<QPair<QString, QJsonValue>> args = {})
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

  namespace mp = mp;

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

serialization::DeserializeResult<Error>
Error::fromJsonObject(const QJsonObject& object)
{
  Error value;

  PARSE_REQUIRED_FIELD_OR_ELSE_ERROR(
    value.description, object, "description", String);

  return value;
}

serialization::SerializeResult
Error::toJsonObject() const
{
  return body("ERROR", { { "description", description } });
}

serialization::SerializeResult
ComputeKey::toJsonObject() const
{
  if (correlationId.isEmpty()) {
    return serialization::StringError("correlationId must be set");
  }

  return body("COMPUTE_KEY", { { "correlationId", correlationId } });
}

serialization::DeserializeResult<ComputeKey>
ComputeKey::fromJsonObject([[maybe_unused]] const QJsonObject& object)
{
  ComputeKey value;

  PARSE_REQUIRED_FIELD_OR_ELSE_ERROR(
    value.correlationId, object, "correlationId", String);

  return value;
}

serialization::DeserializeResult<IntermediateKey>
IntermediateKey::fromJsonObject(const QJsonObject& object)
{
  QString value;
  QString correlationId;

  PARSE_REQUIRED_FIELD_OR_ELSE_ERROR(value, object, "key", String);
  PARSE_REQUIRED_FIELD_OR_ELSE_ERROR(
    correlationId, object, "correlationId", String);

  return IntermediateKey{ .correlationId = correlationId,
                          .key = mp::cpp_int{ value.toUtf8().constData() } };
}

serialization::SerializeResult
IntermediateKey::toJsonObject() const
{
  if (correlationId.isEmpty()) {
    return serialization::StringError("correlationId must be set");
  }

  return body("INTERMEDIATE_KEY",
              { { "correlationId", correlationId },
                { "key", QString::fromStdString(to_string(key)) } });
}

serialization::DeserializeResult<FinalKey>
FinalKey::fromJsonObject(const QJsonObject& object)
{
  QString value;
  PARSE_REQUIRED_FIELD_OR_ELSE_ERROR(value, object, "key", String);

  return FinalKey{ .key = mp::cpp_int{ value.toUtf8().constData() } };
}

serialization::SerializeResult
FinalKey::toJsonObject() const
{
  return body("FINAL_KEY",
              { { "key", QString::fromStdString(to_string(key)) } });
}

}
