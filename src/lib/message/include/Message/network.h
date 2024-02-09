#ifndef LAB4_DIFFIE_HELLMAN_NETWORK_H
#define LAB4_DIFFIE_HELLMAN_NETWORK_H

#include <type_traits>

#include <QJsonDocument>
#include <QLoggingCategory>
#include <QWebSocket>

#include "message.h"
#include "parsing.h"

namespace lab4::message {

namespace logging {
Q_DECLARE_LOGGING_CATEGORY(net)
}

template<typename T>
concept OkCallback = std::is_invocable_v<T, QJsonObject>;

template<message::serialization::JsonSerializable T>
void
trySerialize(QWebSocket* connection,
             const T& message,
             OkCallback auto&& okCallback)
{
  auto result = message.toJsonObject();

  if (auto* err = std::get_if<serialization::StringError>(&result)) {
    qCWarning(logging::net())
      << "Cannot serialize message of type" << T::SerializedName << ':' << *err;
    return;
  }

  const auto& obj = std::get<QJsonObject>(result);
  okCallback(obj);
}

inline void
sendMessage(QWebSocket* connection, const QJsonObject& payload)
{
  connection->sendTextMessage(
    QJsonDocument{ payload }.toJson(QJsonDocument::Compact));
}

template<message::serialization::JsonSerializable T>
void
sendMessage(QWebSocket* connection, const T& message)
{
  trySerialize(connection, message, [connection](const QJsonObject& obj) {
    sendMessage(connection, obj);
  });
}

void
sendError(QWebSocket* connection, QString description);

void
sendError(QWebSocket* connection, const ParseError& error);

}

#endif // LAB4_DIFFIE_HELLMAN_NETWORK_H
