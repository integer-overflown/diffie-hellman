#ifndef LAB4_DIFFIE_HELLMAN_PARSING_UTILS_H
#define LAB4_DIFFIE_HELLMAN_PARSING_UTILS_H

// clang-format off
#define STATEMENT_START do {
#define STATEMENT_END } while(0)
// clang-format on

#define PARSE_REQUIRED_FIELD_OR_ELSE_RETURN(                                   \
  outVariable, obj, name, type, fallbackRet)                                   \
  STATEMENT_START                                                              \
  if (auto field = obj[name]; !field.isUndefined() && field.is##type()) {      \
    outVariable = field.to##type();                                            \
  } else {                                                                     \
    qCWarning(logging::serialization())                                        \
      << "Required field" << name << "is missing";                             \
    return fallbackRet;                                                        \
  }                                                                            \
  STATEMENT_END

#define PARSE_REQUIRED_FIELD_OR_ELSE_ERROR(outVariable, obj, name, type)       \
  PARSE_REQUIRED_FIELD_OR_ELSE_RETURN(                                         \
    outVariable,                                                               \
    obj,                                                                       \
    name,                                                                      \
    type,                                                                      \
    serialization::StringError{ "No such field " #name })

#endif // LAB4_DIFFIE_HELLMAN_PARSING_UTILS_H
