#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;

template <typename type_qi = std::uint32_t, typename type_Ri = std::uint32_t,
          typename type_ni = std::uint32_t>
class ProblemSize {
public:
  using type_q = type_qi;
  using type_R = type_Ri;
  using type_n = type_ni;

  ProblemSize(type_q const qi, type_R const Ri, type_n const ni)
      : q(qi), R(Ri), n(ni) {}

public:
  // Values are read only and public!
  type_q const q;
  type_R const R;
  type_n const n;
};

template <typename type_q = std::uint32_t, typename type_R = std::uint32_t,
          typename type_n = std::uint32_t, typename type_cnt = std::uint64_t>
class Params {
public:
  using ProblemType = ProblemSize<type_q, type_R, type_n>;

  Params(int argc, char *argv[], bool with_cnt = true);

  ProblemType const &get_problem_size() const { return ps; }
  type_cnt get_cnt() const { return cnt; }

private:
  po::variables_map vm;
  ProblemType const ps;
  type_cnt const cnt;
};

namespace {

po::variables_map init_variables_map(int argc, char *argv[]) {
  po::variables_map vm;

  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
      "q", po::value<std::uint32_t>(), "set q - alphabet size")(
      "R", po::value<std::uint32_t>(), "set R - max distance")(
      "n", po::value<std::uint32_t>(), "set n - size / dimension")(
      "cnt", po::value<std::uint64_t>(), "set cnt - number of codes / rooks");
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    exit(1);
  }

  return vm;
}

template <typename type_q, typename type_R, typename type_n>
ProblemSize<type_q, type_R, type_n>
init_problem_type(po::variables_map const &vm) {
  type_q q;
  if (vm.count("q")) {
    q = vm["q"].as<type_q>();
  } else {
    std::cout << "Size (q) was not set.\n";
    exit(1);
  }

  type_R R;
  if (vm.count("R")) {
    R = vm["R"].as<type_R>();
  } else {
    std::cout << "Maximum distance was not set.\n";
    exit(1);
  }

  type_n n;
  if (vm.count("n")) {
    n = vm["n"].as<type_n>();
  } else {
    std::cout << "Size was not set.\n";
    exit(1);
  }

  return ProblemSize<type_q, type_R, type_n>(q, R, n);
}

template <typename type_cnt> type_cnt init_cnt(po::variables_map const &vm) {
  if (vm.count("cnt")) {
    return vm["cnt"].as<type_cnt>();
  } else {
    std::cout << "Count was not set.\n";
    exit(1);
  }
}

} // namespace

template <typename type_q, typename type_R, typename type_n, typename type_cnt>
Params<type_q, type_R, type_n, type_cnt>::Params(int argc, char *argv[],
                                                 bool with_cnt)
    : vm(init_variables_map(argc, argv)),
      ps(init_problem_type<type_q, type_R, type_n>(vm)),
      cnt(with_cnt ? init_cnt<type_cnt>(vm) : 0) {}
