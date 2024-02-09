#include <QCoreApplication>
#include <QLoggingCategory>

#include <cstdlib>
#include <iostream>

#include "client.h"

namespace {

[[noreturn]] void
usage()
{
  std::cerr << "./client_app [server_url] [self_id]";
  exit(EXIT_FAILURE);
}

namespace logging {

Q_LOGGING_CATEGORY(app, "app")

}

}

int
main(int argc, char* argv[])
{
  using namespace lab4::app;

  if (argc != 3) {
    usage();
  }

  QCoreApplication app(argc, argv);

  auto serverUrl = QCoreApplication::arguments()[1];
  auto selfId = QCoreApplication::arguments()[2];

  client::Client client;
  QObject::connect(
    &client, &client::Client::errorOccurred, &app, [](auto error) {
      qCWarning(logging::app()) << "Error occurred:" << error;
    });

  client.connect(serverUrl, selfId);

  return QCoreApplication::exec();
}
