#ifndef LAB4_DIFFIE_HELLMAN_CLIENT_H
#define LAB4_DIFFIE_HELLMAN_CLIENT_H

#include <QObject>
#include <QUrl>
#include <QWebSocket>

#include <queue>

#include "DiffieHellman/diffie_hellman.h"
#include "Message/message.h"
#include "crypto_config.h"

namespace lab4::app::client {

class Client : public QObject
{
  Q_OBJECT

public:
  Client();

  void connect(const QUrl& serverUrl, const QString& selfId);

signals:
  void errorOccurred(QAbstractSocket::SocketError error);

private:
  void handleIncomingMessage(const QString& payload);

  void onConnectionStateChanged(QAbstractSocket::SocketState state);

  struct MessageHandler;
  friend struct MessageHandler;

  template<message::serialization::JsonSerializable T>
  void sendMessage(const T& message);

  void sendMessage(const QJsonObject& payload);

  QWebSocket _socket;
  std::queue<QJsonObject> _pendingMessages;
  diffie_hellman::Config _dhConfig;
};

}

#endif // LAB4_DIFFIE_HELLMAN_CLIENT_H
