#ifndef LAB4_DIFFIE_HELLMAN_MESSAGE_H
#define LAB4_DIFFIE_HELLMAN_MESSAGE_H

#include <variant>

#include <boost/multiprecision/cpp_int.hpp>

#include "serialization.h"

namespace lab4::message {

struct Hello
{
  JSON_SERIALIZABLE_TYPE(Hello)

  QString selfId;
};

struct CryptoSetup
{
  JSON_SERIALIZABLE_TYPE(CryptoSetup)

  boost::multiprecision::cpp_int g;
  boost::multiprecision::cpp_int n;
};

struct Error
{
  JSON_SERIALIZABLE_TYPE(Error)

  QString description;
};

using Message = std::variant<Hello, CryptoSetup, Error>;

}

#endif // LAB4_DIFFIE_HELLMAN_MESSAGE_H
