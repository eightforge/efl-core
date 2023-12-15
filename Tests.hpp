#include <efl/Core/Fundamental.hpp>
#include <efl/Core/Traits.hpp>
#include <efl/Core/Ref.hpp>
#include <efl/Core/Option.hpp>
#if CPPVER_LEAST(17)
# include <efl/Core/Tuple.hpp>
#endif
#include <iostream>

namespace C = efl::C;
namespace HH = efl::C::H;

template <typename...Args>
using And = C::conjunction<Args...>;
template <typename...Args>
using Or = C::disjunction<Args...>;
template <typename Arg>
using Not = C::negation<Arg>;

using Tb = HH::TrueType;
using Fb = HH::FalseType;

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

template <typename T, typename U>
bool compare_all(const T& t, const U& u) {
  bool eq = ((t == u) && (u == t)) ||
    ((t != u) && (u != t));
  bool gt = (t > u) || (u > t) || (t < u) || (u < t);
  bool ge = (t >= u) || (u >= t) || (t <= u) || (u <= t);
  return eq && gt && ge;
}

//=== Tests ===//

// Logical traits
MEflESAssert(And<>::value);
MEflESAssert(And<Tb, Tb, Tb>::value);
MEflESAssert(!And<Tb, Fb, Tb>::value);
MEflESAssert(!And<Fb, Fb, Fb>::value);

MEflESAssert(!Or<>::value);
MEflESAssert(Or<Tb, Tb, Tb>::value);
MEflESAssert(Or<Tb, Fb, Tb>::value);
MEflESAssert(!Or<Fb, Fb, Fb>::value);

MEflESAssert(Not<Fb>::value);
MEflESAssert(!Not<Tb>::value);

// Invokable
MEflESAssert(!C::is_invokable<int>::value);
MEflESAssert(!C::is_invokable<int, float, void*>::value);
MEflESAssert(C::is_invokable<Z, float, void*>::value);

void invoke_tests() {
  auto p = &Z::operator();
  Z1 z1 { };
  auto iz = HH::invoke(p, z1, 1.0f, &z1);
  HH::invoke(&X::operator(), X{});
  assert(iz == 1);
  iz = HH::invoke(z1, 7.9f, &p);
  assert(iz == 7);
}

void ref_tests() {
  Z1 z1 { };
  C::Ref<Z1> rz1 {z1};
  assert(rz1(99.3f, &rz1) == 99);

  static constexpr C::i32 ci = 1;
  constexpr C::Ref<const C::i32> rci {ci};
  constexpr const C::i32& ri = rci.get();
  std::cout << "ri: " << ri << '\n';
}

int option_tests() {
  constexpr C::Option<C::i32> i32_op {1};
  C::i32 i = MEflUnwrap(i32_op) + 4;
  std::cout << "i: " << i << '\n';

  C::Option<std::string> str_op;
  C::Option<const char*> ccp_op {"Hello world!!"};
  str_op = C::make_option<std::string>({"Hello world!"});
  std::cout << "s: " << MEflUnwrap(str_op) << std::endl;

  C::Option<C::f32> f32_op;
  if(f32_op.hasValue()) {
    std::cout << "f: " << MEflUnwrap(f32_op) << '\n';
  }

  C::Option<std::string> swap_op {"Success!"};
  swap_op.swap(swap_op);
  std::swap(str_op, swap_op);

  std::cout << "opt :: cc[]: " 
    << compare_all(str_op, "Hello world!") << '\n';
  std::cout << "opt :: opt<cc*>: " 
    << compare_all(str_op, ccp_op) << '\n';
  std::cout << "opt :: null: " 
    << compare_all(str_op, ccp_op) << '\n';

  std::cout << MEflUnwrap(str_op) << std::endl;
  return 0;
}
