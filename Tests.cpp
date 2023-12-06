#include <efl/Core/Fundamental.hpp>
#include <efl/Core/Traits.hpp>
#include <efl/Core/Ref.hpp>
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

#if CPPVER_LEAST(14)
MEflHasTrait(HasType, typename T::type)
MEflHasValueTrait(HasCallable, T{}())
#endif

#if CPPVER_LEAST(20)
template <char...CC1, char...CC2>
void print_lits(HH::BLitC<char, CC1...> lhs, HH::BLitC<char, CC2...> rhs) {
  std::cout << lhs.Data() << ' ' << rhs.Data() << std::endl;
}
#endif

struct Z {
  int operator()(float f, void*) { return int(f); }
};

struct Z1 : Z { };

int main() {
#if CPPVER_LEAST(14)
  MEflESAssert(!HasType<X>);
  MEflESAssert(HasCallable<X>);
  MEflESAssert(HasType<Y>);
  MEflESAssert(!HasCallable<Y>);
#endif
  
  MEflESAssert(!C::is_invokable<int>::value);
  MEflESAssert(!C::is_invokable<int, float, void*>::value);
  MEflESAssert(C::is_invokable<Z, float, void*>::value);

  auto p = &Z::operator();
  Z1 z1 { };
  auto iz = HH::invoke(p, z1, 1.0f, &z1);
  HH::invoke(&X::operator(), X{});
  assert(iz == 1);
  iz = HH::invoke(z1, 7.9f, &p);
  assert(iz == 7);

  C::Ref<Z1> rz1 {z1};
  assert(rz1(99.3f, &rz1) == 99);

  static constexpr C::i32 ci = 1;
  constexpr C::Ref<const C::i32> rci {ci};
  constexpr const C::i32& ri = rci.get();
  std::cout << "ri: " << ri << '\n';
  
  constexpr C::Option<C::i32> i32_op {ci};
  C::i32 i = MEflUnwrap(i32_op) + 4;
  std::cout << "i: " << i << '\n';

  C::Option<std::string> str_op;
  str_op = "Hello world!";
  std::cout << "s: " << MEflUnwrap(str_op) << '\n';

  C::Option<C::f32> f32_op;
  if(f32_op.hasValue()) {
    std::cout << "f: " << MEflUnwrap(f32_op) << '\n';
  }

  
#if CPPVER_LEAST(20)
  using Lit = HH::LitC<"Hello">;
  constexpr auto lit = C::Ls<"world!">;
  print_lits(Lit{}, lit);
#endif

  std::cout << "Success!" << std::endl;
}