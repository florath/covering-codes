#include "Params.hh"
#include "lib/abn.hh"
#include <cstdint>
#include <cstdio>
#include <gmp.h>

//#define DEBUG

#define IGNORE_TRIVIAL 1

void pcsc(std::uint32_t const qi, std::uint32_t const Ri,
          std::uint32_t const ni) {
  mpz_t q;
  mpz_t R;
  mpz_t n;

  mpz_init(q);
  mpz_init(R);
  mpz_init(n);

  mpz_set_ui(q, qi);
  mpz_set_ui(R, Ri);
  mpz_set_ui(n, ni);

  // Compute code size
  mpz_t codes_cnt;
  mpz_init(codes_cnt);
  mpz_set(codes_cnt, q);

  for (std::uint32_t n_idx(1); n_idx < ni; ++n_idx) {
    mpz_mul(codes_cnt, codes_cnt, q);
  }
#ifdef DEBUG
  printf("--- %u %u %u\n", qi, Ri, ni);
  mpz_out_str(stdout, 10, codes_cnt);
  printf("\n");
#endif

  mpz_t one_code_coverage;
  mpz_init(one_code_coverage);

  if (Ri != 1) {
    // Coverage of one code
    abn<> codes(qi, Ri, ni, 1);
    std::pair<std::uint64_t, std::uint64_t> const cc(codes.covered_cnt());

#ifdef DEBUG
    std::cout << "CC " << cc.first << " " << cc.second << std::endl;
#endif

    mpz_set_ui(one_code_coverage, cc.first);
  } else {

    mpz_set_ui(one_code_coverage, 1);

    mpz_t q_minus_one;
    mpz_init(q_minus_one);
    mpz_sub_ui(q_minus_one, q, 1);

    mpz_t summand;
    mpz_init(summand);
    mpz_set_ui(summand, ni);

    for (std::uint32_t R_idx(0); R_idx < Ri; ++R_idx) {
      mpz_mul(summand, summand, q_minus_one);
#ifdef DEBUG
      printf("Summand ");
      mpz_out_str(stdout, 10, summand);
      printf("\n");
#endif
      mpz_add(one_code_coverage, one_code_coverage, summand);
#ifdef DEBUG
      printf("OCC ");
      mpz_out_str(stdout, 10, one_code_coverage);
      printf("\n");
#endif
    }
  }

#ifdef DEBUG
  mpz_out_str(stdout, 10, one_code_coverage);
  printf("\n");
#endif

  int const divisible(mpz_divisible_p(codes_cnt, one_code_coverage));
#ifdef DEBUG
  printf("DIVISIBLE: %d\n", divisible);
#endif
  if (divisible) {
    mpz_t cnt;
    mpz_init(cnt);
    mpz_divexact(cnt, codes_cnt, one_code_coverage);

    if (!IGNORE_TRIVIAL || mpz_cmp_ui(cnt, 1) != 0) {
      std::cout << qi << " " << Ri << " " << ni << " ";
      mpz_out_str(stdout, 10, cnt);
      printf("\n");
    }
  }
}

void perfect_codes_size_check(std::uint32_t const q, std::uint32_t const R,
                              std::uint32_t const n) {
  for (std::uint32_t q_idx(1); q_idx < q; ++q_idx) {
    for (std::uint32_t R_idx(1); R_idx < R; ++R_idx) {
      for (std::uint32_t n_idx(1); n_idx < n; ++n_idx) {
        pcsc(q_idx, R_idx, n_idx);
      }
    }
  }
}

int main(int argc, char *argv[]) {
  Params<> const params(argc, argv, false);

  perfect_codes_size_check(params.get_problem_size().q,
                           params.get_problem_size().R,
                           params.get_problem_size().n);
  return 0;
}
