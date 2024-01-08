#include "Tests.hpp"
// #include <expected>
#include <string_view>
#include <memory>
#include <efl/Core/Panic.hpp>

// TODO: Add more to Traits/Helpers.hpp
// EFLI_CXX14_CXPR_ void X() { }

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

template <bool HasComma = true>
void gen_apply(const char* name, int n, int start_off = 0) {
  constexpr auto lit = HasComma ? ", " : " ";
  if(n == start_off) return;
  std::cout << name << "(" << start_off << ")";
  for(int idx = (start_off + 1); idx < n; ++idx)
    std::cout << lit << name 
      << "(" << idx << ")";
}

void gen_overload_bases(int count = 32) {
  for(int n = 1; n <= count; ++n) {
    std::cout << "template <";
    gen_apply("TY_", n);
    std::cout << ">\nstruct OverloadSet<";
    gen_apply("N_", n);
    std::cout << ">\n : ";
    gen_apply("N_", n);
    std::cout << " {\n  ";
    gen_apply<false>("OV_", n);
    std::cout << "\n};\n" << std::endl;
  }
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
    std::cout << "res: " << res.unwrap() << std::endl;
  } /* void specialization */ {
    C::Result<void, int> res {};
    assert(res.hasValue());
    res = $Err(5);
    assert(res.error() == 5);
    res.emplace();
    assert(res.hasValue());
  }

  char third_arg = 'a';
  std::cout << "Is multithreaded: " << efl::is_multithreaded() << std::endl;
  // C::panic_();

  std::cout << "Tests:" << std::endl;
  auto tup = std::make_tuple("Hello!", ' ', "I ", 4, 'M', " G", 0, 'D');
  HH::apply(ToApply{}, tup);

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