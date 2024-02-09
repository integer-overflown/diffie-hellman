#include "service.h"

#include <QLoggingCategory>
#include <QWebSocket>

#include "Message/message.h"
#include "Message/network.h"
#include "Message/parsing.h"

#include "crypto_config.h"

namespace lab4::service {

namespace logging {
Q_LOGGING_CATEGORY(service, "service")
}

Service::Service(QObject* parent)
  : QObject(parent)
  // We use custom encryption, so standard security is turned off
  , _server(QStringLiteral("DhMessagingSrv"), QWebSocketServer::NonSecureMode)
{
  QObject::connect(&_server,
                   &QWebSocketServer::newConnection,
                   this,
                   &Service::handleNewConnection);
}

bool
Service::listen(qint16 port)
{
  const bool ret = _server.listen(QHostAddress::Any, port);

  if (!ret) {
    qCCritical(logging::service())
      << "Cannot start listening:" << _server.errorString();
    return false;
  }

  qCDebug(logging::service()) << "Listening on" << _server.serverUrl();
  return true;
}

void
Service::handleNewConnection()
{
  qCDebug(logging::service()) << "Have new connection";

  auto* newConnection = _server.nextPendingConnection();

  QObject::connect(newConnection,
                   &QWebSocket::textMessageReceived,
                   this,
                   [this, sender = newConnection](const QString& message) {
                     handleMessage(sender, message);
                   });

  QObject::connect(
    newConnection, &QWebSocket::disconnected, [this, sender = newConnection] {
      handleDisconnect(sender);
    });

  QObject::connect(
    newConnection,
    &QWebSocket::errorOccurred,
    [this, sender = newConnection](QAbstractSocket::SocketError error) {
      handleError(sender, error);
    });
}

template<typename Any>
void
Service::MessageHandler::operator()(const Any&)
{
  qCWarning(logging::service())
    << "Cannot handle message of type" << Any::SerializedName;
}

void
Service::MessageHandler::operator()(const message::Hello& message)
{
  qCDebug(logging::service()) << "Received Hello from" << message.selfId;

  service->_users.emplace(connection, message.selfId);

  const auto& config = crypto_config::loadDefault();
  message::sendMessage(connection,
                       message::CryptoSetup{ .g = config.g, .n = config.n });
}

void
Service::handleMessage(QWebSocket* sender, const QString& payload)
{
  auto parseResult = message::parse(payload);

  if (auto* err = std::get_if<message::ParseError>(&parseResult)) {
    qCWarning(logging::service()) << "Could not parse message:" << *err;
    message::sendError(sender, *err);
    return;
  }

  auto& message = std::get<message::Message>(parseResult);

  std::visit(MessageHandler{ this, sender }, message);
}

void
Service::removeConnection(QWebSocket* connection)
{
  qCDebug(logging::service()) << "Removing connection" << connection;

  auto it = _users.find(connection);

  if (it == _users.end()) {
    qCDebug(logging::service()) << "Connection hasn't been registered";
    connection->deleteLater();
    return;
  }

  const auto& id = it->second;

  qCDebug(logging::service()) << "Peer ID was" << id;

  connection->deleteLater();
  _users.erase(it);
}

void
Service::handleDisconnect(QWebSocket* sender)
{
  qCDebug(logging::service()) << sender << "disconnected";
  removeConnection(sender);
}

void
Service::handleError(QWebSocket* sender, QAbstractSocket::SocketError error)
{
  qCWarning(logging::service())
    << "Connection" << sender << "has encountered an error" << error;
  removeConnection(sender);
}

}
