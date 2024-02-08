#ifndef LAB4_DIFFIE_HELLMAN_FUNCTIONAL_H
#define LAB4_DIFFIE_HELLMAN_FUNCTIONAL_H

namespace lab4::utils {

template<typename... Args>
struct Overloaded : Args...
{
  using Args::operator()...;
};

template<typename... Args>
Overloaded(Args...) -> Overloaded<Args...>;

}

#endif // LAB4_DIFFIE_HELLMAN_FUNCTIONAL_H
