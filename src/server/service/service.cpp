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

struct Service::MessageHandler
{
  Service* service;
  QWebSocket* connection;

  template<typename Any>
  void operator()(const Any&) const
  {
    qCWarning(logging::service())
      << "Cannot handle message of type" << Any::SerializedName;
  }

  void operator()(const message::Hello& message) const
  {
    qCDebug(logging::service()) << "Received Hello from" << message.selfId;

    if (!service->registerConnection(connection, message.selfId)) {
      message::sendError(
        connection,
        QStringLiteral("Name '%1' is already registered").arg(message.selfId));
      return;
    }

    const auto& config = crypto_config::loadDefault();
    message::sendMessage(connection,
                         message::CryptoSetup{ .g = config.g, .n = config.n });

    const auto& peers = service->_state.peers;

    if (peers.size() < 2) {
      return;
    }

    qCDebug(logging::service())
      << "Starting handshake for" << peers.size() << "users";

    for (const auto& [peer, socket] : peers) {
      qCDebug(logging::service()) << "Sending COMPUTE_KEY to" << peer;
      message::sendMessage(socket, message::ComputeKey{});
    }
  }
};

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

bool
Service::registerConnection(QWebSocket* connection, const QString& peerId)
{
  qCDebug(logging::service())
    << "Assigning connection" << connection << "to peer" << peerId;

  if (_state.peers.contains(peerId)) {
    qCDebug(logging::service())
      << "Peer name" << peerId << "is already registered";
    return false;
  }

  _state.peers.emplace(peerId, connection);
  _state.connections.emplace(connection, peerId);
  return true;
}

void
Service::removeConnection(QWebSocket* connection)
{
  qCDebug(logging::service()) << "Removing connection" << connection;

  auto& [connections, peers] = _state;
  auto it = connections.find(connection);

  if (it == connections.end()) {
    qCDebug(logging::service()) << "Connection hasn't been registered";
    connection->deleteLater();
    return;
  }

  const auto& id = it->second;

  qCDebug(logging::service()) << "Peer ID was" << id;

  connection->deleteLater();
  peers.erase(id);
  connections.erase(it);
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
