#ifndef LAB4_DIFFIE_HELLMAN_MESSAGE_H
#define LAB4_DIFFIE_HELLMAN_MESSAGE_H

#include <variant>

#include <boost/multiprecision/cpp_int.hpp>

#include "serialization.h"

namespace lab4::message {

class Hello
{
  JSON_SERIALIZABLE_TYPE(Hello)

public:
  QString selfId;
};

class CryptoSetup
{
  JSON_SERIALIZABLE_TYPE(CryptoSetup)

public:
  boost::multiprecision::cpp_int g;
  boost::multiprecision::cpp_int n;
};

using Message = std::variant<Hello, CryptoSetup>;

}

#endif // LAB4_DIFFIE_HELLMAN_MESSAGE_H
