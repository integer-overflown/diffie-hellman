#include "registry.h"

#define MATCH_TYPE(string, type)                                               \
  {                                                                            \
    u##string##_s, [](const QJsonObject& obj) -> RetType {                     \
      return convert(DESERIALIZE_FROM_JSON(obj, type));                        \
    }                                                                          \
  }

namespace lab4::message::registry {

namespace {
template<typename T>
auto
convert(const serialization::DeserializeResult<T>& variant)
  -> std::variant<Message, serialization::StringError>
{
  if (auto* err = std::get_if<serialization::StringError>(&variant)) {
    return *err;
  }

  return Message{ std::get<T>(variant) };
};
}

const RegistryType&
load()
{
  using namespace Qt::StringLiterals;

  static std::unordered_map<QString, ParseFn> registry = {
    MATCH_TYPE("HELLO", Hello),
    MATCH_TYPE("CRYPTO_SETUP", CryptoSetup),
    MATCH_TYPE("ERROR", Error),
    MATCH_TYPE("COMPUTE_KEY", ComputeKey),
    MATCH_TYPE("INTERMEDIATE_KEY", IntermediateKey),
  };

  return registry;
}

}
