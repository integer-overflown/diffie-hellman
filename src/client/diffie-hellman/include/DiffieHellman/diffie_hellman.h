#include <cstdint>

#include <boost/multiprecision/cpp_int.hpp>

namespace lab4::diffie_hellman {

using BigInt = boost::multiprecision::cpp_int;

struct Config
{
  BigInt g;
  BigInt n;
  std::uint8_t exp;
};

[[nodiscard]] BigInt calculateKey(const Config& config);

}
