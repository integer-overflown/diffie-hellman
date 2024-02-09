#include "diffie_hellman.h"

#include <iostream>

namespace lab4::diffie_hellman
{

namespace mp = boost::multiprecision;

BigInt
calculateKey(const Config& config)
{
  return mp::pow(config.g, config.exp) % (config.n);
}

}
