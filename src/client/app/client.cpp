#include "client.h"

#include <QLoggingCategory>

#include "Message/message.h"
#include "Message/network.h"

namespace lab4::app::client {

namespace logging {
Q_LOGGING_CATEGORY(client, "app.client")
}

Client::Client()
  : _dhConfig{ .exp = crypto_config::PrivateKey::generate() }
{
  QObject::connect(
    &_socket, &QWebSocket::errorOccurred, this, &Client::errorOccurred);
  QObject::connect(&_socket,
                   &QWebSocket::textMessageReceived,
                   this,
                   &Client::handleIncomingMessage);
  QObject::connect(&_socket,
                   &QWebSocket::stateChanged,
                   this,
                   &Client::onConnectionStateChanged);
}

void
Client::sendMessage(const QJsonObject& payload)
{
  if (_socket.state() == QAbstractSocket::ConnectedState) {
    message::sendMessage(&_socket, payload);
    return;
  }

  qCDebug(logging::client()) << "Not connected yet, queueing";
  _pendingMessages.push(payload);
}

template<message::serialization::JsonSerializable T>
void
Client::sendMessage(const T& message)
{
  message::trySerialize(
    &_socket, message, [this](const QJsonObject& obj) { sendMessage(obj); });
}

void
Client::connect(const QUrl& serverUrl, const QString& selfId)
{
  qCDebug(logging::client()) << "Joining to" << serverUrl;
  qCDebug(logging::client()) << "Self ID is" << selfId;

  _socket.open(serverUrl);
  sendMessage(message::Hello{ .selfId = selfId });
}

struct Client::MessageHandler
{
  Client* client;

  template<typename Any>
  void operator()(const Any& message)
  {
    qCWarning(logging::client())
      << "Received unsupported message of type" << Any::SerializedName;
  }

  void operator()(const message::CryptoSetup& message)
  {
    qCDebug(logging::client()) << "Received CryptoSetup message";

    auto& dh = client->_dhConfig;
    dh =
      diffie_hellman::Config{ .g = message.g, .n = message.n, .exp = dh.exp };
  }

  void operator()(const message::Error& message)
  {
    qCCritical(logging::client())
      << "Received error from server:" << message.description;
  }

  void operator()(const message::ComputeKey&)
  {
    qDebug(logging::client()) << "Received ComputeKey";
    client->sendMessage(message::IntermediateKey{
      .key = diffie_hellman::calculateKey(client->_dhConfig) });
  }
};

void
Client::handleIncomingMessage(const QString& payload)
{
  auto parseResult = message::parse(payload);

  if (auto* err = std::get_if<message::ParseError>(&parseResult)) {
    qCWarning(logging::client()) << "Failed to parse incoming message";
    message::sendError(&_socket, *err);
    return;
  }

  const auto& message = std::get<message::Message>(parseResult);
  std::visit(MessageHandler{ this }, message);
}

void
Client::onConnectionStateChanged(QAbstractSocket::SocketState state)
{
  qCDebug(logging::client()) << "Connection state changed to" << state;

  if (state != QAbstractSocket::ConnectedState) {
    return;
  }

  qCInfo(logging::client()) << "Connected";
  qCDebug(logging::client())
    << "Sending" << _pendingMessages.size() << "pending messages";

  while (!_pendingMessages.empty()) {
    auto& message = _pendingMessages.front();
    message::sendMessage(&_socket, message);
    _pendingMessages.pop();
  }
}
}
