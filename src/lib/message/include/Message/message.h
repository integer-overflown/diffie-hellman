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

struct ComputeKey
{
  JSON_SERIALIZABLE_TYPE(ComputeKey)
};

struct IntermediateKey
{
  boost::multiprecision::cpp_int key;

  JSON_SERIALIZABLE_TYPE(IntermediateKey)
};

using Message =
  std::variant<Hello, CryptoSetup, Error, ComputeKey, IntermediateKey>;

}

#endif // LAB4_DIFFIE_HELLMAN_MESSAGE_H
