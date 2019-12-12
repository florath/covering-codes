#include <vector>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <cassert>
#include <stdexcept>
#include <string>

#define INPARAM_q 3
#define INPARAM_R 1
#define INPARAM_n 4

#define INPARAM_cnt 9

class abn {
public:
  abn(std::uint32_t const dim, std::uint32_t const base, std::uint32_t const cnt=1)
    : m_dim(dim), m_cnt(cnt), m_base(base), m_data(m_dim * m_cnt, 0) {}

  void init_different_pos() {
    abn pos(INPARAM_n, INPARAM_q);
    for(int i(m_cnt -1); i>=0; --i) {
      for(int j(0); j<m_dim; ++j) {
	m_data[ i * m_dim + j] = pos.m_data[j];
      }
      ++pos;
    }
  }

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

  void yaml() {
    std::ofstream o(std::string("ub-") + std::to_string(INPARAM_cnt) + "-generated.yaml");
    o << "---" << std::endl
      << "q: " << INPARAM_q << std::endl
      << "R: " << INPARAM_R << std::endl
      << "n: " << INPARAM_n << std::endl
      << "upper_bound: " << INPARAM_cnt << std::endl
      << "generator: \"cc-gen-solution V0.0.1\"" << std::endl
      << "solution:" << std::endl;
    for(int i(0); i<m_cnt; ++i) {
      o << "  - [";
      for(int j(0); j<m_dim; ++j) {
	o << m_data[ i * m_dim + j];
	if(j!=m_dim-1) {
	  o << ", ";
	}
      }
      o << "]" << std::endl;
    }
    o.close();
  }

  std::ostream & print(std::ostream & out) const {
    out << "[" << m_dim << "," << m_cnt << "," << m_base << ": ";
    for(auto const & data : m_data) {
      out << data << " ";
    }
    out << "]";
    return out;
  }

  abn & operator++() {
    for(std::uint32_t i(0); i<m_data.size(); ++i) {
      if(m_data[i] < m_base - 1) {
	++m_data[i];
	return *this;
      } else {
	m_data[i] = 0;
      }
    }
    throw std::out_of_range("++abn exhausted");
  }

  int hamming_distance(int partitioning, abn const & pos) const {
    int hd(0);

    assert(m_dim == pos.m_dim);
    assert(m_base == pos.m_base);
    
    for(int i(0); i<m_dim; ++i) {
      if(m_data[partitioning * m_dim + i]!=pos.m_data[i]) {
	++hd;
      }
    }
    return hd;
  }

private:
  std::uint32_t const m_dim;
  std::uint32_t const m_cnt;
  std::uint32_t const m_base;
  std::vector<std::uint32_t> m_data;
};

std::ostream & operator<<(std::ostream & out, abn const & a) {
  return a.print(out);
}

bool check_max_dist(abn const & rooks) {
  abn pos(INPARAM_n, INPARAM_q);

  
  try {
  
  while(true) {

    int min_hd(INPARAM_n);
    
    for(std::uint32_t i(0); i<INPARAM_cnt; ++i) {
      int const hd(rooks.hamming_distance(i, pos));

      // std::cout << "HD from " << rooks << " part " << i << " to " << pos << " is " << hd << std::endl;

      if(hd < min_hd){
	min_hd = hd;
      }

      if(min_hd < INPARAM_R) {
	break;
      }
    }

    //std::cout << "Min hd " << min_hd << std::endl;

    if(min_hd > INPARAM_R) {
      //std::cout << "FAIL" << std::endl;
      return false;
    }
    
    //std::cout << pos << std::endl;
    ++pos;

  }
  } catch(std::out_of_range & e) {
  }
  return true;
}

int main() {
  abn rooks(INPARAM_n, INPARAM_q, INPARAM_cnt);

  //  rooks.init_different_pos();
  rooks.init_perfect();

  std::uint32_t round(0);

  while(true) {
    if(check_max_dist(rooks)) {
      std::cout << "HURRAY " << rooks << std::endl;
      rooks.yaml();
      break;
    }
    //    std::cout << rooks << std::endl;
    ++rooks;

    ++round;
    if(round%10000000 == 0) {
      std::cout << "Status " << rooks << std::endl;
    }
    
    
  }

  return 0;
}
