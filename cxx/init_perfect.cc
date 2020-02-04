/*
   g++ -o init_perfect -O3 init_perfect.cc -l boost_program_options
 */

#include "abn.hh"
#include "Params.hh"

int main(int argc, char *argv[]) {
  Params<> const params(argc, argv);

  abn<> rooks(params.get_problem_size().q, params.get_problem_size().R,
              params.get_problem_size().n, params.get_cnt());
  // rooks.init_different_pos();
  rooks.init_perfect();

  std::cout << "Init perfect finished" << std::endl;

  if (rooks.complete_covered()) {
    std::cout << "Complete coverage" << std::endl;
    return 0;
  } else {
    std::cout << "NOT Complete coverage" << std::endl;
    return 1;
  }
  return 2;
}
