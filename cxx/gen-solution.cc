/*
   g++ -o gen-solution -O3 gen-solution.cc -l boost_program_options
 */
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

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

  Params(int argc, char *argv[]);

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
    std::cout << "Compression level was not set.\n";
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
Params<type_q, type_R, type_n, type_cnt>::Params(int argc, char *argv[])
    : vm(init_variables_map(argc, argv)),
      ps(init_problem_type<type_q, type_R, type_n>(vm)),
      cnt(init_cnt<type_cnt>(vm)) {}

// CLASS abn

template <typename type_q = std::uint32_t, typename type_R = std::uint32_t,
          typename type_n = std::uint32_t, typename type_cnti = std::uint64_t>
class abn {
public:
  using type_cnt = type_cnti;

  abn(ProblemSize<type_q, type_R, type_n> const &psi, type_cnt const cnti)
      : cnt(cnti), ps(psi), data(ps.n * cnt, 0) {}

  void init_different_pos() {
    abn<type_q, type_R, type_n, type_cnt> pos(ps, 1);
    for (int i(cnt - 1); i >= 0; --i) {
      copy_data(i, pos, 0);
      ++pos;
    }
  }

  // Check if there is a double cover of codes[0:cn] for field
  bool is_double_covered(type_cnt const cn, abn const &field) {
    type_cnt covered(0);
    for (type_cnt code(0); code <= cn; ++code) {
      if (hamming_distance(code, field, 0) <= ps.R) {
        ++covered;
        if (covered > 1) {
#ifdef TRACE
          std::cout << "double cover" << std::endl;
#endif
          return true;
        }
      }
    }
    return false;
  }

  // Check if there is a double cover of codes[0:cn] for complete field
  bool is_double_covered(type_cnt const cn) {
    abn field(ps, 1);
    // For each field
    try {
      while (true) {
#ifdef TRACE
        std::cout << "TRACE ";
        print(std::cout);
        std::cout << std::endl;
        std::cout << "      ";
        field.print(std::cout);
        std::cout << std::endl;
#endif
        if (is_double_covered(cn, field)) {
          return true;
        }
        ++field;
      }
    } catch (std::out_of_range &oor) {
#ifdef DEBUG
      std::cout << "CATCH FIELD ";
      print(std::cout);
      std::cout << std::endl;
#endif
      return false;
    }
    abort();
  }

  void init_perfect() {
    // For number of codes
    abn cur_code(ps, 1);
    for (type_cnt cn(0); cn < cnt; ++cn) {
      std::cout << cn << " " << std::flush;
#ifdef DEBUG
      std::uint32_t loop_cnt(0);
#endif

      while (true) {

#ifdef DEBUG
        ++loop_cnt;
        if (loop_cnt % 1000 == 0) {
          std::cout << cn << " CURCODE ";
          cur_code.print(std::cout);
          std::cout << std::endl;
        }
#endif
        copy_data(cn, cur_code, 0);
#ifdef TRACE
        std::cout << cn << " AFTERCOPY ";
        print(std::cout);
        std::cout << std::endl;
#endif

        if (is_double_covered(cn)) {
#ifdef TRACE
          std::cout << "INC CUR CODE" << std::endl;
#endif
          try {
            ++cur_code;
          } catch (std::out_of_range &oor) {
            std::cout << "CATCH CUR CODE" << std::endl;
            abort();
          }
        } else {
          break;
        }
      }
    }
    std::cout << "The End" << std::endl;
    print(std::cout);
    std::cout << std::endl;
    abort();
  }

  void yaml() {
    std::ofstream o(std::string("ub-") + std::to_string(ps.q) + "-" +
                    std::to_string(ps.R) + "-" + std::to_string(ps.n) + "-" +
                    std::to_string(cnt) + "-generated.yaml");
    o << "---" << std::endl
      << "q: " << ps.q << std::endl
      << "R: " << ps.R << std::endl
      << "n: " << ps.n << std::endl
      << "upper_bound: " << cnt << std::endl
      << "generator: \"cc-gen-solution V0.0.2\"" << std::endl
      << "solution:" << std::endl;
    for (int i(0); i < cnt; ++i) {
      o << "  - [";
      for (int j(0); j < ps.n; ++j) {
        o << data[i * ps.n + j];
        if (j != ps.n - 1) {
          o << ", ";
        }
      }
      o << "]" << std::endl;
    }
    o.close();
  }

  std::ostream &print(std::ostream &out) const {
    out << "[" << ps.q << "/" << ps.R << "/" << ps.n << "-" << cnt << ": ";
    for (auto const &d : data) {
      out << d << " ";
    }
    out << "]";
    return out;
  }

  abn &operator++() {
    for (std::uint32_t i(0); i < data.size(); ++i) {
      if (data[i] < ps.q - 1) {
        ++data[i];
        return *this;
      } else {
        data[i] = 0;
      }
    }
    throw std::out_of_range("++abn exhausted");
  }

  type_n hamming_distance(type_n const local_idx, abn const &that,
                          type_n const that_idx) const {
    type_n hd(0);

    assert(ps.n == that.ps.n);
    assert(ps.q == that.ps.q);

    for (type_n i(0); i < ps.n; ++i) {
      if (data[local_idx * ps.n + i] != that.data[that_idx * ps.n + i]) {
        ++hd;
      }
    }
    return hd;
  }

private:
  // Sets the local data at index 'idx' to the that objects' that_idx value.
  inline void copy_data(type_n const local_idx, abn const &that,
                        type_n const that_idx) {
    for (type_n j(0); j < ps.n; ++j) {
      data[local_idx * ps.n + j] = that.data[that_idx * ps.n + j];
    }
  }

  type_cnt cnt;
  ProblemSize<type_q, type_R, type_n> const ps;
  std::vector<type_n> data;
};

template <typename type_q = std::uint32_t, typename type_R = std::uint32_t,
          typename type_n = std::uint32_t, typename type_cnt = std::uint64_t>
std::ostream &operator<<(std::ostream &out,
                         abn<type_q, type_R, type_n, type_cnt> const &a) {
  return a.print(out);
}

template <typename type_q = std::uint32_t, typename type_R = std::uint32_t,
          typename type_n = std::uint32_t, typename type_cnt = std::uint64_t>
bool check_max_dist(abn<type_q, type_R, type_n, type_cnt> const &rooks,
                    ProblemSize<type_q, type_R, type_n> const &ps,
                    type_cnt const cnt) {
  abn<type_q, type_R, type_n, type_cnt> pos(ps, cnt);

  try {
    while (true) {

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
      ++pos;
    }
  } catch (std::out_of_range &e) {
  }
  return true;
}

int main(int argc, char *argv[]) {
  Params<> const params(argc, argv);

  abn<> rooks(params.get_problem_size(), params.get_cnt());
  // rooks.init_different_pos();
  rooks.init_perfect();

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
