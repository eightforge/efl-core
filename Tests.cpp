#include "Tests.hpp"
#include "Codegen.hpp"
// #include <expected>
#include <string_view>
#include <memory>
#include <efl/Core/Panic.hpp>

struct AnnotationTest {
  int first = 0;
  float second = 3.0f;
  void* third = nullptr;
};

struct ToApply {
private:
  template <typename T>
  ALWAYS_INLINE static C::ibyte ID(T&& t) NOEXCEPT {
    std::cout << t << ' ';
    return C::ibyte(0);
  }

  template <typename...TT>
  ALWAYS_INLINE static CXX11Void Ignore(TT&&...) NOEXCEPT {
    CXX11Return();
  }

public:
  template <typename...TT>
  void operator()(TT&&...tt) CONST {
    ToApply::Ignore(
      ToApply::ID(HH::cxpr_forward<TT>(tt))...);
    std::cout << std::endl;
  }
};

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

auto result_test(int i) 
 -> C::Result<C::Str, C::ubyte> {
  if(i >= 0) return $Ok(std::to_string(i));
  else return $Err(C::ubyte(i));
}

int main() {
#if CPPVER_LEAST(14)
  MEflESAssert(!HasType<X>);
  MEflESAssert(HasCallable<X>);
  MEflESAssert(HasType<Y>);
  MEflESAssert(!HasCallable<Y>);
#endif
  MEflESAssert(C::is_nothrow_convertible<int, float>::value);
  std::cout << std::boolalpha;

  C::Poly<MyBase, Meower, Woofer> poly { };
  poly.asBase();
  poly = Meower();
  poly->saySomething();
  poly = Woofer();
  poly->saySomething();
  poly.clear();
  assert(!poly.holdsAny());

  /* default instantiation */ {
    auto res = result_test(5);
    assert(res.unwrap() == "5");
    res = result_test(-3);
    assert(!res.hasValue());
    res = result_test(453);
    auto unwrapped = $unwrap(res);
    std::cout << "res: " << unwrapped << std::endl;
  } /* void specialization */ {
    C::Result<void, int> res {};
    assert(res.hasValue());
    res = $Err(5);
    assert(res.error() == 5);
    res.emplace();
    assert(res.hasValue());
  }

  std::cout << "Is multithreaded: " << efl::is_multithreaded() << std::endl;
  // C::panic_();

  std::cout << "Tests:" << std::endl;
  auto tup = C::make_tuple("Hello!", ' ', "I ", 4, 'M', " G", 0, 'D');
  HH::apply(ToApply{}, tup);

  int i = 0;
  auto arr3 = C::make_array(i, 1, 2);
  auto arr2 = C::make_array_of<C::Str>("0", "1");
  auto arr0 = C::make_array();

#if CPPVER_LEAST(20)
  using Lit = HH::LitC<"Hello">;
  constexpr auto lit = C::Ls<"world!">;
  print_lits(Lit{}, lit);
#endif

  invoke_tests();
  ref_tests();
  strref_tests();
  return option_tests();
}