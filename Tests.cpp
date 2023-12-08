#include "Tests.hpp"

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

  std::cout << std::boolalpha;
  std::cout << "Tests:" << std::endl;
  
#if CPPVER_LEAST(20)
  using Lit = HH::LitC<"Hello">;
  constexpr auto lit = C::Ls<"world!">;
  print_lits(Lit{}, lit);
#endif

  invoke_tests();
  ref_tests();
  return option_tests();
}