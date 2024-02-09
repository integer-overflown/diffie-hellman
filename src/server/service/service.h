#ifndef LAB4_DIFFIE_HELLMAN_SERVICE_H
#define LAB4_DIFFIE_HELLMAN_SERVICE_H

#include <map>
#include <set>
#include <unordered_map>

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

  [[nodiscard]] bool registerConnection(QWebSocket* connection,
                                        const QString& peerId);
  void removeConnection(QWebSocket* connection);

  struct MessageHandler;
  friend struct MessageHandler;

  QWebSocketServer _server;

  struct
  {
    std::unordered_map<QWebSocket*, QString> connections;
    std::map<QString, QWebSocket*> peers;
  } _state;

  std::unordered_map<QString, std::vector<QString>> _keyExchangeMap;
};

}

#endif // LAB4_DIFFIE_HELLMAN_SERVICE_H
