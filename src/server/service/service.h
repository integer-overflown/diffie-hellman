#ifndef LAB4_DIFFIE_HELLMAN_SERVICE_H
#define LAB4_DIFFIE_HELLMAN_SERVICE_H

#include <QObject>
#include <QWebSocketServer>

#include "Message/message.h"

namespace lab4::service {

class Service : public QObject
{
  Q_OBJECT

public:
  static inline constexpr qint16 DefaultPort = 8000;

  explicit Service(QObject* parent = nullptr);

  [[nodiscard]] bool listen(qint16 port = DefaultPort);

private:
  void handleNewConnection();
  void handleMessage(QWebSocket* sender, const QString& payload);
  void handleDisconnect(QWebSocket* sender);
  void handleError(QWebSocket* sender, QAbstractSocket::SocketError error);

  void removeConnection(QWebSocket* connection);

  friend struct MessageHandler;

  struct MessageHandler
  {
    Service* service;
    QWebSocket* connection;

    void operator()(const message::Hello& message);

    template<typename Any>
    void operator()(const Any&);
  };

  QWebSocketServer _server;
  std::unordered_map<QWebSocket*, QString> _users;
};

}

#endif // LAB4_DIFFIE_HELLMAN_SERVICE_H
