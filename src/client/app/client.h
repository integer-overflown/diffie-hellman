#ifndef LAB4_DIFFIE_HELLMAN_CLIENT_H
#define LAB4_DIFFIE_HELLMAN_CLIENT_H

#include <QObject>
#include <QUrl>
#include <QWebSocket>

#include <queue>

#include "Message/message.h"

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

  friend struct MessageHandler;

  struct MessageHandler
  {
    Client* client;

    void operator()(const message::CryptoSetup& message);

    template<typename Any>
    void operator()(const Any& message);
  };

  template<message::serialization::JsonSerializable T>
  void sendMessage(const T& message);

  void sendMessage(const QJsonObject& payload);

  QWebSocket _socket;
  std::queue<QJsonObject> _pendingMessages;
};

}

#endif // LAB4_DIFFIE_HELLMAN_CLIENT_H
