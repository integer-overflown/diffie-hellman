#ifndef LAB4_DIFFIE_HELLMAN_SERVICE_H
#define LAB4_DIFFIE_HELLMAN_SERVICE_H

#include <QObject>
#include <QWebSocketServer>

namespace lab4::service {

class Service : public QObject
{
  Q_OBJECT

public:
  explicit Service(QObject* parent = nullptr);

private:
  void handleNewConnection();
  void handleMessage(QWebSocket* sender, const QString& message);
  void handleDisconnect(QWebSocket* sender);
  void handleError(QWebSocket* sender, QAbstractSocket::SocketError error);

  void removeConnection(QWebSocket *connection);

  QWebSocketServer _server;
  std::unordered_map<QWebSocket*, QString> _users;
};

}

#endif // LAB4_DIFFIE_HELLMAN_SERVICE_H
