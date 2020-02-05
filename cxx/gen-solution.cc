/*
   g++ -o gen-solution -O3 gen-solution.cc -l boost_program_options
 */
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "Params.hh"
#include "abn.hh"

template <typename type_q = std::uint32_t, typename type_R = std::uint32_t,
          typename type_n = std::uint32_t, typename type_cnt = std::uint64_t>
bool check_max_dist(abn<type_q, type_R, type_n, type_cnt> const &rooks,
                    ProblemSize<type_q, type_R, type_n> const &ps,
                    type_cnt const cnt) {
  for (abn<type_q, type_R, type_n, type_cnt> pos(ps.q, ps.R, ps.n, cnt); pos.not_exhausted();
       ++pos) {
    type_n min_hd(ps.n);

    for (std::uint32_t i(0); i < cnt; ++i) {
      int const hd(rooks.hamming_distance(i, pos, 0));

      // std::cout << "HD from " << rooks << " part " << i << " to " << pos <<
      // " is " << hd << std::endl;

      if (hd < min_hd) {
        min_hd = hd;
      }

      if (min_hd < ps.R) {
        break;
      }
    }

    // std::cout << "Min hd " << min_hd << std::endl;

    if (min_hd > ps.R) {
      // std::cout << "FAIL" << std::endl;
      return false;
    }

    // std::cout << pos << std::endl;
  }
  return true;
}

int main(int argc, char *argv[]) {
  Params<> const params(argc, argv);

  abn<> rooks(params.get_problem_size().q, params.get_problem_size().R,
              params.get_problem_size().n, params.get_cnt());
  // rooks.init_different_pos();
  rooks.init_perfect();

  std::cout << "Init perfect finished" << std::endl;
  abort();

  std::uint32_t round(0);

  while (true) {
    if (check_max_dist(rooks, params.get_problem_size(), params.get_cnt())) {
      std::cout << "HURRAY " << rooks << std::endl;
      rooks.yaml();
      break;
    }
    //    std::cout << rooks << std::endl;
    ++rooks;

    ++round;
    if (round % 10000000 == 0) {
      std::cout << "Status " << rooks << std::endl;
    }
  }

  return 0;
}
