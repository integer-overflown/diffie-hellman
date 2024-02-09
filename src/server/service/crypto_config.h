#ifndef LAB4_DIFFIE_HELLMAN_CRYPTO_CONFIG_H
#define LAB4_DIFFIE_HELLMAN_CRYPTO_CONFIG_H

#include <boost/multiprecision/cpp_int.hpp>

namespace lab4::service::crypto_config {

struct CryptoConfig
{
  boost::multiprecision::cpp_int g;
  boost::multiprecision::cpp_int n;
};

const CryptoConfig&
loadDefault();

}

#endif // LAB4_DIFFIE_HELLMAN_CRYPTO_CONFIG_H
