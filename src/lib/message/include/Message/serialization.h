#ifndef LAB4_DIFFIE_HELLMAN_SERIALIZATION_H
#define LAB4_DIFFIE_HELLMAN_SERIALIZATION_H

#include <optional>
#include <utility>

#include <QJsonObject>

#define JSON_DECLARE_SERIALIZABLE(Type)                                        \
  static constexpr auto SerializedName = #Type;                                \
                                                                               \
  [[nodiscard]] ::lab4::message::serialization::SerializeResult toJsonObject() \
    const;

#define JSON_DECLARE_DESERIALIZABLE(Type)                                      \
  [[nodiscard]] static ::lab4::message::serialization::DeserializeResult<Type> \
  fromJsonObject(const QJsonObject& object);

#define SERIALIZE_TO_JSON(value)                                               \
  ::lab4::message::serialization::JsonSerializer::serialize(value)

#define DESERIALIZE_FROM_JSON(object, type)                                    \
  ::lab4::message::serialization::JsonDeserializer<type>::deserialize(object)

#define JSON_SERIALIZABLE_TYPE(Type)                                           \
  JSON_DECLARE_SERIALIZABLE(Type)                                              \
  JSON_DECLARE_DESERIALIZABLE(Type)

namespace lab4::message::serialization {

class StringError
{
public:
  explicit StringError(QString cause)
    : _cause(std::move(cause))
  {
  }

  [[nodiscard]] QString cause() const { return _cause; }

private:
  QString _cause;
};

template<typename T>
using DeserializeResult = std::variant<StringError, T>;

using SerializeResult = std::variant<StringError, QJsonObject>;

template<typename T>
concept JsonSerializable = requires(T t) {
  {
    t.toJsonObject()
  } -> std::same_as<SerializeResult>;
  {
    T::SerializedName
  } -> std::same_as<const char*>;
};

template<typename T>
concept JsonDeserializable = requires(T t, QJsonObject obj) {
  {
    t.fromJsonObject(obj)
  } -> std::same_as<DeserializeResult<T>>;
};

template<typename T>
struct JsonSerializer
{
  [[nodiscard]] static SerializeResult serialize(const T& t)
    requires JsonSerializable<T>
  {
    return t.toJsonObject();
  }
};

template<typename T>
struct JsonDeserializer
{
  [[nodiscard]] static DeserializeResult<T> deserialize(
    const QJsonObject& object)
    requires JsonDeserializable<T>
  {
    return T::fromJsonObject(object);
  }
};

}

#endif // LAB4_DIFFIE_HELLMAN_SERIALIZATION_H
