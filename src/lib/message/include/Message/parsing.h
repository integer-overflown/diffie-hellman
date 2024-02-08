#ifndef LAB4_DIFFIE_HELLMAN_PARSING_H
#define LAB4_DIFFIE_HELLMAN_PARSING_H

#include <QDebug>
#include <QJsonParseError>

#include <variant>

#include "message.h"
#include "serialization.h"

namespace lab4::message {
struct ParseError
{
public:
  struct UnsupportedType
  {
    QString type;

    friend QDebug& operator<<(QDebug& out, const UnsupportedType& value);
  };

  struct JsonParseError
  {
    QJsonParseError error;

    friend QDebug& operator<<(QDebug& out, const JsonParseError& value);
  };

  struct InvalidDocument
  {
    friend QDebug& operator<<(QDebug& out, const InvalidDocument& value);
  };

  struct NoTypeSpec
  {
    friend QDebug& operator<<(QDebug& out, const InvalidDocument& value);
  };

  struct MessageParsingFailed
  {
    serialization::StringError error;

    friend QDebug& operator<<(QDebug& out, const MessageParsingFailed& value);
  };

  using ErrorKind = std::variant<UnsupportedType,
                                 JsonParseError,
                                 InvalidDocument,
                                 NoTypeSpec,
                                 MessageParsingFailed>;

  explicit ParseError(ErrorKind kind)
    : _kind(std::move(kind))
  {
  }

  friend QDebug& operator<<(QDebug& out, const ParseError& value);

private:
  ErrorKind _kind;
};

using ParseResult = std::variant<Message, ParseError>;

[[nodiscard]] ParseResult
parse(const QString& string);

}

#endif // LAB4_DIFFIE_HELLMAN_PARSING_H
