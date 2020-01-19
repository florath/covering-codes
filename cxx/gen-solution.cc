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

class Params {
public:
  Params(int argc, char *argv[]);

  std::uint32_t get_q() const { return q; }
  std::uint32_t get_R() const { return R; }
  std::uint32_t get_n() const { return n; }

  std::uint64_t get_cnt() const { return cnt; }

private:
  std::uint32_t q;
  std::uint32_t R;
  std::uint32_t n;

  std::uint64_t cnt;
};

Params::Params(int argc, char *argv[]) {
  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
      "q", po::value<std::uint32_t>(), "set q - alphabet size")(
      "R", po::value<std::uint32_t>(), "set R - max distance")(
      "n", po::value<std::uint32_t>(), "set n - size / dimension")(
      "cnt", po::value<std::uint64_t>(), "set cnt - number of codes / rooks");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    exit(1);
  }

  if (vm.count("q")) {
    q = vm["q"].as<std::uint32_t>();
  } else {
    std::cout << "Compression level was not set.\n";
    exit(1);
  }

  if (vm.count("R")) {
    R = vm["R"].as<std::uint32_t>();
  } else {
    std::cout << "Maximum distance was not set.\n";
    exit(1);
  }

  if (vm.count("n")) {
    n = vm["n"].as<std::uint32_t>();
  } else {
    std::cout << "Size was not set.\n";
    exit(1);
  }

  if (vm.count("cnt")) {
    cnt = vm["cnt"].as<std::uint64_t>();
  } else {
    std::cout << "Count was not set.\n";
    exit(1);
  }
}

class abn {
public:
  abn(Params const &params)
      : m_params(params), m_data(m_params.get_n() * m_params.get_cnt(), 0) {}

  void init_different_pos() {
    abn pos(m_params);
    for (int i(m_params.get_cnt() - 1); i >= 0; --i) {
      for (int j(0); j < m_params.get_n(); ++j) {
        m_data[i * m_params.get_n() + j] = pos.m_data[j];
      }
      ++pos;
    }
  }

#if 0
  void init_perfect() {
    abn pos(INPARAM_n, INPARAM_q);
    for(int i(m_cnt -1); i>=0; --i) {
      for(int j(0); j<m_dim; ++j) {
	m_data[ i * m_dim + j] = pos.m_data[j];
      }
      do {
	++pos;

	for(int k(m_cnt-1); k>=i; --k) {
	  dfkajsdflkajsdf kj 
	}
	
	std::cout << "HD " << perfect.hamming_distance(1, pos) << std::endl;
      }	while(perfect.hamming_distance(0, pos)<=INPARAM_R);
    }
  }
#endif

  void yaml() {
    std::ofstream o(std::string("ub-") + std::to_string(m_params.get_q()) +
                    "-" + std::to_string(m_params.get_R()) + "-" +
                    std::to_string(m_params.get_n()) + "-" +
                    std::to_string(m_params.get_cnt()) + "-generated.yaml");
    o << "---" << std::endl
      << "q: " << m_params.get_q() << std::endl
      << "R: " << m_params.get_R() << std::endl
      << "n: " << m_params.get_n() << std::endl
      << "upper_bound: " << m_params.get_cnt() << std::endl
      << "generator: \"cc-gen-solution V0.0.2\"" << std::endl
      << "solution:" << std::endl;
    for (int i(0); i < m_params.get_cnt(); ++i) {
      o << "  - [";
      for (int j(0); j < m_params.get_n(); ++j) {
        o << m_data[i * m_params.get_n() + j];
        if (j != m_params.get_n() - 1) {
          o << ", ";
        }
      }
      o << "]" << std::endl;
    }
    o.close();
  }

  std::ostream &print(std::ostream &out) const {
    out << "[" << m_params.get_n() << "," << m_params.get_cnt() << ","
        << m_params.get_q() << ": ";
    for (auto const &data : m_data) {
      out << data << " ";
    }
    out << "]";
    return out;
  }

  abn &operator++() {
    for (std::uint32_t i(0); i < m_data.size(); ++i) {
      if (m_data[i] < m_params.get_q() - 1) {
        ++m_data[i];
        return *this;
      } else {
        m_data[i] = 0;
      }
    }
    throw std::out_of_range("++abn exhausted");
  }

  int hamming_distance(int partitioning, abn const &pos) const {
    int hd(0);

    assert(m_params.get_n() == pos.m_params.get_n());
    assert(m_params.get_q() == pos.m_params.get_q());

    for (int i(0); i < m_params.get_n(); ++i) {
      if (m_data[partitioning * m_params.get_n() + i] != pos.m_data[i]) {
        ++hd;
      }
    }
    return hd;
  }

private:
  Params const m_params;
  std::vector<std::uint32_t> m_data;
};

std::ostream &operator<<(std::ostream &out, abn const &a) {
  return a.print(out);
}

bool check_max_dist(abn const &rooks, Params const &params) {
  abn pos(params);

  try {

    while (true) {

      int min_hd(params.get_n());

      for (std::uint32_t i(0); i < params.get_cnt(); ++i) {
        int const hd(rooks.hamming_distance(i, pos));

        // std::cout << "HD from " << rooks << " part " << i << " to " << pos <<
        // " is " << hd << std::endl;

        if (hd < min_hd) {
          min_hd = hd;
        }

        if (min_hd < params.get_R()) {
          break;
        }
      }

      // std::cout << "Min hd " << min_hd << std::endl;

      if (min_hd > params.get_R()) {
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

  Params const params(argc, argv);

  abn rooks(params);
  rooks.init_different_pos();
  // rooks.init_perfect();

  std::uint32_t round(0);

  while (true) {
    if (check_max_dist(rooks, params)) {
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
