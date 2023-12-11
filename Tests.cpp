#include "Tests.hpp"
#include <efl/Core/Mtx.hpp>
#include <efl/Core/Panic.hpp>
#include <expected>

namespace XX11 = HH::xx11;

#define XXASSERT(cond) do { __glibcxx_assert_impl(cond); } while (false)

struct ToApply {
private:
  template <typename T>
  ALWAYS_INLINE static C::ibyte ID(T&& t) NOEXCEPT {
    std::cout << t << ' ';
  }

  template <typename...TT>
  ALWAYS_INLINE static void Ignore(TT&&...) NOEXCEPT { }

public:
  template <typename...TT>
  constexpr void operator()(TT&&...tt) CONST {
    ToApply::Ignore( 
      ToApply::ID(EFLI_CXPRFWD_(tt))...);
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
  
  MEflESAssert(!C::is_invokable<int>::value);
  MEflESAssert(!C::is_invokable<int, float, void*>::value);
  MEflESAssert(C::is_invokable<Z, float, void*>::value);

  std::cout << std::boolalpha;
  std::cout << "Tests:" << std::endl;

  auto tup = std::make_tuple("Hello!", ' ', "I ", 4, 'M', " G", 0, 'D');
  XX11::apply(ToApply{}, tup);
  
#if CPPVER_LEAST(20)
  using Lit = HH::LitC<"Hello">;
  constexpr auto lit = C::Ls<"world!">;
  print_lits(Lit{}, lit);
#endif

  invoke_tests();
  ref_tests();
  return option_tests();
}