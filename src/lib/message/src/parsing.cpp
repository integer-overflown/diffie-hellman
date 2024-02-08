#include "parsing.h"

#include "logging.h"
#include "parsing_utils.h"

namespace lab4::message {

QDebug&
operator<<(QDebug& out, const ParseError::UnsupportedType& value)
{
  return out << "Unsupported message type" << value.type;
}

QDebug&
operator<<(QDebug& out, const ParseError::JsonParseError& value)
{
  return out << "Json parse error occurred:" << value.error.errorString();
}

QDebug&
operator<<(QDebug& out, const ParseError::InvalidDocument& value)
{
  return out << "JSON document must be an object";
}

QDebug&
operator<<(QDebug& out, const ParseError::NoTypeSpec& value)
{
  return out << "Message does not contains a valid type spec: field 'type' "
                "must be a valid string denoting message type";
}

QDebug&
operator<<(QDebug& out, const ParseError::MessageParsingFailed& value)
{
  return out << "Message parsing failed" << value.error.cause();
}

QDebug&
operator<<(QDebug& out, const ParseError& value)
{
  return std::visit([&out](const auto& val) -> QDebug& { return out << val; },
                    value._kind);
}

namespace {

#define MATCH_TYPE(string, type)                                               \
  {                                                                            \
    u##string##_s, [](const QJsonObject& obj) -> RetType {                     \
      return convert(DESERIALIZE_FROM_JSON(obj, type));                        \
    }                                                                          \
  }

template<typename T>
auto
convert(const serialization::DeserializeResult<T>& variant)
  -> std::variant<Message, serialization::StringError>
{
  if (auto* err = std::get_if<serialization::StringError>(&variant)) {
    return *err;
  }

  return Message{ std::get<T>(variant) };
};

std::variant<Message, serialization::StringError>
parseMessage(const QString& type, const QJsonObject& payload)
{
  using namespace Qt::StringLiterals;
  using RetType = std::variant<Message, serialization::StringError>;
  using ParseFn = RetType (*)(const QJsonObject&);

  static std::unordered_map<QString, ParseFn> typeMap = {
    MATCH_TYPE("HELLO", Hello),
    MATCH_TYPE("CRYPTO_SETUP", CryptoSetup),
  };

  return typeMap[type](payload);
}
}

ParseResult
parse(const QString& string)
{
  qCDebug(logging::trace()) << "Parsing" << string;

  QJsonParseError error;

  auto document = QJsonDocument::fromJson(string.toUtf8(), &error);

  if (error.error != QJsonParseError::NoError) {
    return ParseError{ ParseError::JsonParseError{ error } };
  }

  if (!document.isObject()) {
    return ParseError{ ParseError::InvalidDocument{} };
  }

  auto object = document.object();

  QString type;
  PARSE_REQUIRED_FIELD_OR_ELSE_RETURN(
    type, object, "type", String, ParseError{ ParseError::NoTypeSpec{} });

  auto ret = parseMessage(type, object);

  if (auto* err = std::get_if<serialization::StringError>(&ret)) {
    return ParseError{ ParseError::MessageParsingFailed{ *err } };
  }

  return std::get<Message>(ret);
}

}
