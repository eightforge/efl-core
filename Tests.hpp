#include <efl/Core.hpp>
#include <iostream>

namespace C = efl::core;
namespace HH = efl::core::H;

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

struct MyBase {
  virtual ~MyBase() {}
  virtual void saySomething() = 0;
};

struct Meower : MyBase {
  void saySomething() override { std::printf("Meow!\n"); }
};

struct Woofer : MyBase {
  void saySomething() override { std::printf("Woof!\n"); }
};

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
  $raw_assert(iz == 1);
  iz = HH::invoke(z1, 7.9f, &p);
  $raw_assert(iz == 7);
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

void strref_tests() {
  static constexpr char lit[] = "Hello!";
  constexpr C::StrRef str(lit);
  constexpr char lc = C::StrRef(lit)[0];
  MEflESAssert(lc == 'H');

  constexpr C::StrRef sl = 
    str.snipPrefix(2).snipSuffix(2);
  MEflESAssert(sl[0] == 'l' && sl[1] == 'l');
}

void poly_tests() {
  C::Poly<MyBase, Meower, Woofer> poly { };
  (void)poly.asBase();
  poly = Meower();
  $raw_assert(poly.holdsAny());
  poly->saySomething();
  poly = Woofer();
  $raw_assert(poly.holdsType<Woofer>());
  poly->saySomething();
  poly.clear();
  $raw_assert(!poly.holdsAny());
}

auto result_test2(int i) 
 -> C::Result<C::Str, C::ubyte> {
  if(i >= 0) return $Ok(std::to_string(i));
  else return $Err(C::ubyte(i));
}

int result_tests() {
  /* default instantiation */ {
    auto res = result_test2(5);
    assert(res.unwrap() == "5");
    res = result_test2(-3);
    assert(!res.hasValue());
    res = result_test2(453);
    auto unwrapped = $unwrap(res, 1);
    std::cout << "res: " << unwrapped << std::endl;
  } /* void specialization */ {
    C::Result<void, int> res {};
    $raw_assert(res.hasValue());
    res = $Err(5);
    $raw_assert(res.error() == 5);
    res.emplace();
    $raw_assert(res.hasValue());
  }
  return 0;
}

void array_tests() {
  int i = 0;
  auto arr3 = C::make_array(i, 1, 2);
  auto arr2 = C::make_array_of<C::Str>("0", "1");
  auto arr0 = C::make_array();
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
