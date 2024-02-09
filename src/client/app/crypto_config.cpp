#include "crypto_config.h"

#include <random>

namespace lab4::app::crypto_config {

PrivateKey::KeyType
PrivateKey::generate()
{

  std::random_device rd;
  std::independent_bits_engine<std::mt19937, sizeof(KeyType) * 8, KeyType> gen(
    rd());
  return gen();
}

}
