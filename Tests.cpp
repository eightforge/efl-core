#include "Tests.hpp"
// #include <expected>
#include <string_view>
#include <memory>
// #include <efl/Core/_Version.hpp>

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

int main() {
#if CPPVER_LEAST(14)
  MEflESAssert(!HasType<X>);
  MEflESAssert(HasCallable<X>);
  MEflESAssert(HasType<Y>);
  MEflESAssert(!HasCallable<Y>);
#endif
  MEflESAssert(C::is_nothrow_convertible<int, float>::value);
  std::cout << std::boolalpha;

  char third_arg = 'a';
  std::cout << "Is multithreaded: " << efl::is_multithreaded() << std::endl;
  // auto annotated = std::make_unique<AnnotationTest>(77, 9.0f, &third_arg);
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