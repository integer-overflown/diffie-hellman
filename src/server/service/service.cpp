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

namespace {

template<typename Key, typename Value>
std::vector<Key>
keyVector(const std::map<Key, Value>& map)
{
  std::vector<Key> res;
  res.reserve(map.size());

  for (const auto& [key, _] : map) {
    res.push_back(key);
  }

  return res;
}

QString
makeCorrelationId(const QString& peerId)
{
  return "correlation-" + peerId;
}

std::unordered_map<QString, std::vector<QString>>
generateKeyExchangeMap(std::vector<QString> peers)
{
  std::unordered_map<QString, std::vector<QString>> result;
  std::vector<QString> chain;

  chain.resize(peers.size() - 1);

  for (auto i = 0; i < peers.size(); ++i) {
    auto key = makeCorrelationId(peers[i]);

    for (auto j = 0; j < chain.size(); ++j) {
      chain[j] = peers[(i + j + 1) % peers.size()];
    }

    result.emplace(key, chain);
  }

  return result;
}

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

    service->_keyExchangeMap = generateKeyExchangeMap(keyVector(peers));

    qCDebug(logging::service()) << "-- begin: key exchange map dump --";
    for (const auto& [key, path] : service->_keyExchangeMap) {
      qCDebug(logging::service()) << key << ':' << path;
    }
    qCDebug(logging::service()) << "-- end: key exchange map dump --";

    for (const auto& [peer, socket] : peers) {
      qCDebug(logging::service()) << "Sending COMPUTE_KEY to" << peer;

      auto correlationId = makeCorrelationId(peer);

      message::sendMessage(
        socket, message::ComputeKey{ .correlationId = correlationId });
    }
  }

  void operator()(const message::IntermediateKey& message)
  {
    const auto& [connections, peers] = service->_state;

    auto it = connections.find(connection);

    if (it == connections.end()) {
      qCDebug(logging::service()) << "Peer hasn't been registered yet";
      message::sendError(connection,
                         "Invalid operation: you haven't authorized first: "
                         "make sure to initiate the communication with HELLO "
                         "message before any other message is sent");
      return;
    }

    qCDebug(logging::service())
      << "Got intermediate key in correlation" << message.correlationId;

    auto pathIt = service->_keyExchangeMap.find(message.correlationId);

    if (pathIt == service->_keyExchangeMap.end()) {
      qCDebug(logging::service())
        << "No path registered for correlation" << message.correlationId;
      message::sendError(connection,
                         "Invalid operation: negotiation is already completed");
      return;
    }

    auto& chain = pathIt->second;
    Q_ASSERT(!chain.empty()); // a paranoid check, this should be ensured by the
                              // protocol already

    auto nextPeer = *chain.begin();
    qCDebug(logging::service()) << "Next peer is" << nextPeer;

    chain.erase(chain.begin());

    auto nextPeerConnection = peers.find(nextPeer);

    if (nextPeerConnection == peers.end()) {
      qCWarning(logging::service())
        << "Broken invariant: cannot find connection assigned to" << nextPeer;
      return;
    }

    if (chain.empty()) {
      qCDebug(logging::service()) << "Peer" << nextPeer << "is a final peer";
      message::sendMessage(nextPeerConnection->second,
                           message::FinalKey{ .key = message.key });
      service->_keyExchangeMap.erase(pathIt);
    } else {
      qCDebug(logging::service())
        << "Continuing negotiation, peers left:" << chain.size();
      message::sendMessage(nextPeerConnection->second, message);
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
  _keyExchangeMap.erase(id);
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
