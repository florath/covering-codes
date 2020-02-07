/*
 * Arbitray Base Number
 *
 * Class which handles numbers with (mostly) arbitray bases.
 */
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

// #define TRACE

template <typename q_t = std::uint32_t, typename R_t = std::uint32_t,
          typename n_t = std::uint32_t, typename cnt_ti = std::uint64_t>
class abn {
public:
  using cnt_t = cnt_ti;

  abn(q_t const &qi, R_t const &Ri, n_t const &ni, cnt_t const cnti)
      : cnt(cnti), q(qi), R(Ri), n(ni), data(n * cnt, 0), exhausted(false) {}

  void init_different_pos() {
    abn<q_t, R_t, n_t, cnt_t> pos(q, R, n, 1);
    for (int i(cnt - 1); i >= 0; --i) {
      copy_data(i, pos, 0);
      ++pos;
    }
  }

  // Check if there is a double cover of codes[0:cn) for field
  bool is_double_covered(cnt_t const cn, abn const &field,
                         n_t const field_idx = 0) {
    cnt_t covered(0);
    for (cnt_t code_idx(0); code_idx < cn; ++code_idx) {
      n_t const hd(hamming_distance(code_idx, field, field_idx));
#ifdef TRACE
      std::cout << "Hamming distance [" << hd << "]" << std::endl;
#endif
      if (hd <= R) {
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
  bool is_double_covered(cnt_t const cn) {
    for (abn field(q, R, n, 1); field.not_exhausted(); ++field) {
#ifdef TRACE
      std::cout << "TRACE ";
      print(std::cout);
      std::cout << std::endl;
      std::cout << "      ";
      field.print(std::cout);
      std::cout << std::endl;
#endif
      std::cerr << "INTERFACE CHANGE" << std::endl;
      abort();
      if (is_double_covered(cn, field)) {
        return true;
      }
    }
#ifdef DEBUG
    std::cout << "CATCH FIELD ";
    print(std::cout);
    std::cout << std::endl;
#endif
    return false;
  }

  void init_perfect() {
    // For number of codes
    abn cur_code(q, R, n, 1);
    for (cnt_t code_pos(0); code_pos < cnt; ++code_pos) {
      if (not cur_code.not_exhausted()) {
        std::cerr << "DOES NOT WORK OUT 1" << std::endl;
        abort();
      }
      if (cur_code.last_valid()) {
        std::cerr << "DOES NOT WORK OUT 2" << std::endl;
        abort();
      }
      for (; cur_code.not_exhausted(); ++cur_code) {
        std::cout << "Code Position: " << code_pos << " " << std::flush;
#ifdef DEBUG
        std::uint32_t loop_cnt(0);
#endif

#ifdef DEBUG
        ++loop_cnt;
        if (loop_cnt % 1000 == 0) {
          std::cout << code_pos << " CURCODE ";
          cur_code.print(std::cout);
          std::cout << std::endl;
        }
#endif
        copy_data(code_pos, cur_code, 0);
#ifdef TRACE
        std::cout << code_pos << " AFTERCOPY ";
        print(std::cout);
        std::cout << std::endl;
#endif
        std::cerr << "INTERFACE CHANGE" << std::endl;
        abort();
        if (!is_double_covered(code_pos)) {
          std::cout << "NOT DOUBLE COVERED" << std::endl;
          break;
        }
        std::cout << "DOUBLE COVERTED" << std::endl;
      }
    }
    for (int c(0); c < cnt; ++c) {
      std::cout << "Checking " << c << ": " << std::endl;
      std::cerr << "INTERFACE CHANGE" << std::endl;
      abort();
      if (is_double_covered(c)) {
        std::cout << "NOTHING FOUND 3" << std::endl;
        abort();
      }
    }

    std::cout << "The End" << std::endl;
    print(std::cout);
    std::cout << std::endl;
  }

  void yaml() {
    std::ofstream o(std::string("ub-") + std::to_string(q) + "-" +
                    std::to_string(R) + "-" + std::to_string(n) + "-" +
                    std::to_string(cnt) + "-generated.yaml");
    o << "---" << std::endl
      << "q: " << q << std::endl
      << "R: " << R << std::endl
      << "n: " << n << std::endl
      << "upper_bound: " << cnt << std::endl
      << "generator: \"cc-gen-solution V0.0.3\"" << std::endl
      << "solution:" << std::endl;
    for (int i(0); i < cnt; ++i) {
      o << "  - [";
      for (int j(0); j < n; ++j) {
        o << data[i * n + j];
        if (j != n - 1) {
          o << ", ";
        }
      }
      o << "]" << std::endl;
    }
    o.close();
  }

  std::ostream &print(std::ostream &out) const {
    out << "[" << q << "/" << R << "/" << n << "-" << cnt << ": ";
    for (auto const &d : data) {
      out << d << " ";
    }
    out << "]";
    return out;
  }

  abn &operator++() {
    if (exhausted) {
      throw std::runtime_error("Cannot inc exhausted abn");
    }
    for (std::uint32_t i(0); i < data.size(); ++i) {
      if (data[i] < q - 1) {
        ++data[i];
        return *this;
      } else {
        data[i] = 0;
      }
    }
    exhausted = true;
    return *this;
  }

  bool not_exhausted() const { return not exhausted; }

  bool last_valid() const {
    abn temp(*this);
    ++temp;
    return not temp.not_exhausted();
  }

  n_t hamming_distance(n_t const local_idx, abn const &that,
                       n_t const that_idx) const {
    n_t hd(0);

    assert(n == that.n);
    assert(q == that.q);

    for (n_t i(0); i < n; ++i) {
      if (data[local_idx * n + i] != that.data[that_idx * n + i]) {
        ++hd;
      }
    }
    return hd;
  }

  /*
   * Computes the minimum hamming distance of the current data / code
   * to the complete set of fields.
   */
  n_t hamming_distance_min(abn const &that, n_t const that_idx) const {
    n_t hdm(n);
    for (cnt_t c(0); c < cnt; ++c) {
      n_t const hd(hamming_distance(c, that, that_idx));
      if (hd < hdm) {
        hdm = hd;
      }
    }
    return hdm;
  }

  bool complete_covered() const {
    for (abn field(q, R, n, 1); field.not_exhausted(); ++field) {
      n_t const hdm(hamming_distance_min(field, 0));
      std::cout << "HDM ";
      field.print(std::cout);
      std::cout << " [" << hdm << "]" << std::endl;
      if (hdm > R) {
        std::cerr << "Hamming distance min [" << hdm << "]" << std::endl;
        abort();
      }
    }
    return true;
  }

  // Sets the local data at index 'idx' to the that objects' that_idx value.
  inline void copy_data(n_t const local_idx, abn const &that,
                        n_t const that_idx) {
    for (n_t j(0); j < n; ++j) {
      data[local_idx * n + j] = that.data[that_idx * n + j];
    }
  }

  n_t get(n_t const idx) { return data[idx]; }

private:
  cnt_t const cnt;
  q_t const q;
  R_t const R;
  n_t const n;
  std::vector<n_t> data;
  bool exhausted;
};

template <typename type_q = std::uint32_t, typename type_R = std::uint32_t,
          typename type_n = std::uint32_t, typename type_cnt = std::uint64_t>
std::ostream &operator<<(std::ostream &out,
                         abn<type_q, type_R, type_n, type_cnt> const &a) {
  return a.print(out);
}
