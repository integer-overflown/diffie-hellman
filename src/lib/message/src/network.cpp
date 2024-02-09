#include "network.h"

namespace lab4::message {

void
sendError(QWebSocket* connection, QString description)
{

  sendMessage(connection,
              message::Error{ .description = std::move(description) });
}

void
sendError(QWebSocket* connection, const ParseError& error)
{
  QString description;
  QDebug stream(&description);

  stream << error;

  sendError(connection, std::move(description));
}

}
