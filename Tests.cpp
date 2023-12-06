#include <efl/Core/Fundamental.hpp>
#include <efl/Core/Traits.hpp>
#include <efl/Core/Option.hpp>
// #include <efl/Core/Tuple.hpp>

#include <iostream>
// #include <optional>

namespace C = efl::C;
namespace HH = efl::C::H;

struct X {
  void operator()() { }
};

struct Y {
  using type = X;
};

// MEflHasTrait(HasType, typename T::type)
// MEflHasValueTrait(HasCallable, T{}())

#if CPPVER_LEAST(20)
template <char...CC1, char...CC2>
void print_lits(HH::BLitC<char, CC1...> lhs, HH::BLitC<char, CC2...> rhs) {
  std::cout << lhs.Data() << ' ' << rhs.Data() << std::endl;
}
#endif

int main() {
  MEflCppverMatch(11);
  // MEflESAssert(!HasType<X>);
  // MEflESAssert(HasCallable<X>);
  // MEflESAssert(HasType<Y>);
  // MEflESAssert(!HasCallable<Y>);
  MEflESAssert(std::is_same<
    MEflGTy(Y), typename Y::type>::value);
  
  constexpr C::Option<int> int_op {5};
  int i = MEflUnwrap(int_op);
  std::cout << "i: " << i << '\n';

  C::Option<std::string> str_op;
  str_op = "Hello world!";
  std::cout << "s: " << MEflUnwrap(str_op) << '\n';

  C::Option<float> float_op;
  float f = MEflUnwrap(float_op);
  std::cout << "f: " << f << '\n';

  
#if CPPVER_LEAST(20)
  using Lit = HH::LitC<"Hello">;
  constexpr auto lit = C::Ls<"world!">;
  print_lits(Lit{}, lit);
#endif

  std::cout << "Success!" << std::endl;
}