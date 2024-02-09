#include "service.h"

#include <QCoreApplication>

int
main(int argc, char* argv[])
{
  using namespace lab4;

  QCoreApplication app(argc, argv);
  service::Service service;

  if (!service.listen()) {
    return EXIT_FAILURE;
  }

  return QCoreApplication::exec();
}
