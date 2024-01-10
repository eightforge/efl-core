#include "Tests.hpp"
#include "Codegen.hpp"
// #include <efl/Core/Panic.hpp>

enum class MyEnum { 
  A, BB, CCC, DDDD, EEEEE,
  MEflEnumEnd(EEEEE)
};

enum FlagEnum { 
  E1 = 1, E2 = 2, E3 = 4, E4 = 8,
  MEflFlagEnd(E4)
};

enum class FlagEnumI { 
  E1 = 1, E2 = 2, E3 = 4, E4 = 8
};

namespace efl::C {
  MEflDeclareFlags(FlagEnumI, E4);
} // namespace efl::C

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
  std::cout << "Is multithreaded: " << efl::is_multithreaded() << std::endl;
  std::cout << "Tests:" << std::endl;

  auto tup = C::make_tuple("Hello!", ' ', "I ", 4, 'M', " G", 0, 'D');
  HH::apply(ToApply{}, tup);

#if CPPVER_LEAST(20)
  using Lit = HH::LitC<"Hello">;
  constexpr auto lit = C::Ls<"world!">;
  print_lits(Lit{}, lit);
#endif

  invoke_tests();
  ref_tests();
  strref_tests();
  poly_tests();
  assert(result_tests() == 0);
  array_tests();
  return option_tests();
}