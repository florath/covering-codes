#include "Params.hh"
#include "lib/abn.hh"

int main(int argc, char *argv[]) {
  Params<> const params(argc, argv);

  abn<> rooks(params.get_problem_size().q, params.get_problem_size().R,
              params.get_problem_size().n, params.get_cnt());
  // rooks.init_different_pos();
  rooks.init_perfect();

  std::cout << "Init perfect finished" << std::endl;

  std::pair<std::uint64_t, std::uint64_t> const covered(rooks.covered_cnt());
  std::cout << "Not covered [" << covered.second << "]" << std::endl;

  if (covered.second == 0) {
    std::cout << "Complete coverage" << std::endl;
    return 0;
  } else {
    std::cout << "NOT Complete coverage" << std::endl;
    return 1;
  }
  return 2;
}
