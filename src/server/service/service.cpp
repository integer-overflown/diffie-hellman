#include "service.h"

#include <QLoggingCategory>
#include <QWebSocket>

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

void
Service::handleNewConnection()
{
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

void
Service::handleMessage(QWebSocket* sender, const QString& message)
{
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

  const auto &id = it->second;

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
