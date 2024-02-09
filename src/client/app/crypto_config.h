#ifndef LAB4_DIFFIE_HELLMAN_CRYPTO_CONFIG_H
#define LAB4_DIFFIE_HELLMAN_CRYPTO_CONFIG_H

#include <cstdint>

namespace lab4::app::crypto_config {

struct PrivateKey
{
  using KeyType = std::uint8_t;
  using Self = PrivateKey;
  static KeyType generate();
};

}

#endif // LAB4_DIFFIE_HELLMAN_CRYPTO_CONFIG_H
