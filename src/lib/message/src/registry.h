#ifndef LAB4_DIFFIE_HELLMAN_TYPEMAP_H
#define LAB4_DIFFIE_HELLMAN_TYPEMAP_H

#include "Message/message.h"
#include "Message/serialization.h"

namespace lab4::message::registry {

using RetType = std::variant<Message, serialization::StringError>;
using ParseFn = RetType (*)(const QJsonObject&);
using RegistryType = std::unordered_map<QString, ParseFn>;

[[nodiscard]] const RegistryType&
load();

}

#endif // LAB4_DIFFIE_HELLMAN_TYPEMAP_H
