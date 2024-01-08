#pragma once

#include <iostream>

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

void gen_tuple_bases(int count = 32) {
  for(int n = 1; n <= count; ++n) {
    std::cout << "template <";
    gen_apply("TY_", n);
    // std::cout << ">\nstruct TupleBranch<IdSeq<";
    // gen_apply("ID_", n);
    // std::cout << ">, ";
    std::cout << ">\nstruct TupleBranch<";
    gen_apply("ID_", n);
    std::cout << ">\n : ";
    gen_apply("N_", n);
    std::cout << " {\n" <<
      "  static constexpr bool isArray_ = false;\n"
      "  static constexpr SzType size = " << n << ";\n"
      "  TupleBranch() = default;\n"
      "  TupleBranch(const TupleBranch&) = default;\n"
      "  TupleBranch(TupleBranch&&) = default;\n  ";
    std::cout << "constexpr TupleBranch(";
    gen_apply("ARG_", n);
    std::cout << ")\n   : ";
    gen_apply("IN_", n);
    std::cout << " { }\n};\n" << std::endl;
  }
}