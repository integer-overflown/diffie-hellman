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

  QString correlationId;
};

struct IntermediateKey
{
  JSON_SERIALIZABLE_TYPE(IntermediateKey)

  QString correlationId;
  boost::multiprecision::cpp_int key;
};

struct FinalKey
{
  JSON_SERIALIZABLE_TYPE(FinalKey)

  boost::multiprecision::cpp_int key;
};

using Message = std::
  variant<Hello, CryptoSetup, Error, ComputeKey, IntermediateKey, FinalKey>;

}

#endif // LAB4_DIFFIE_HELLMAN_MESSAGE_H
