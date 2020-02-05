#define BOOST_TEST_MODULE abn_test
#include <boost/test/included/unit_test.hpp>

#include "lib/abn.hh"

BOOST_AUTO_TEST_CASE(test_cnt_2_1_2) {
  std::uint64_t cnt(0);
  for (abn<> field(2, 1, 2, 1); field.not_exhausted(); ++field) {
    ++cnt;
  }
  BOOST_TEST(cnt == 4);
}

BOOST_AUTO_TEST_CASE(test_cnt_3_1_5) {
  std::uint64_t cnt(0);
  for (abn<> field(3, 1, 5, 1); field.not_exhausted(); ++field) {
    ++cnt;
  }
  BOOST_TEST(cnt == 243); // 3 ^ 5
}

BOOST_AUTO_TEST_CASE(test_copy_data_01) {
  abn<> code(3, 1, 5, 4);

  abn<> field(3, 1, 5, 1);
  for (int fp(0); fp < 4; ++fp) {
    ++field;
    code.copy_data(fp, field, 0);
  }

  // std::cout << code << std::endl;

  BOOST_TEST(code.get(0) == 1);
  for (int i(1); i < 5; ++i) {
    BOOST_TEST(code.get(i) == 0);
  }

  BOOST_TEST(code.get(5) == 2);
  for (int i(6); i < 11; ++i) {
    BOOST_TEST(code.get(i) == 0);
  }

  BOOST_TEST(code.get(11) == 1);
  for (int i(12); i < 15; ++i) {
    BOOST_TEST(code.get(i) == 0);
  }
  BOOST_TEST(code.get(15) == 1);
  BOOST_TEST(code.get(16) == 1);
  for (int i(17); i < 20; ++i) {
    BOOST_TEST(code.get(i) == 0);
  }
}

BOOST_AUTO_TEST_CASE(test_hamming_distance_01) {
  abn<> code(3, 1, 3, 5);

  abn<> field(3, 1, 3, 1);
  for (int fp(0); fp < 4; ++fp) {
    ++field;
    code.copy_data(fp, field, 0);
  }

  int const results[] = {1, 1, 1, 2, 0, 1, 2, 1, 1, 0, 2, 2, 2, 2, 0, 1, 1, 2,
                         1, 0, 2, 1, 1, 1, 2, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 2,
                         2, 2, 2, 3, 1, 2, 3, 2, 2, 1, 3, 3, 3, 3, 1, 2, 2, 3,
                         2, 1, 3, 2, 2, 2, 3, 3, 2, 3, 2, 3, 3, 2, 3, 2, 3, 3,
                         2, 2, 2, 3, 1, 2, 3, 2, 2, 1, 3, 3, 3, 3, 1, 2, 2, 3,
                         2, 1, 3, 2, 2, 2, 3, 3, 2, 3, 2, 3, 3, 2, 3, 2, 3, 3};
  int ri(0);

  for (abn<> df(3, 1, 3, 1); df.not_exhausted(); ++df) {
    for (int fp(0); fp < 4; ++fp) {
      // std::cout << "HD " << df << " -> " << code << " [" << fp << "]"
      // << std::endl;
      BOOST_TEST(code.hamming_distance(fp, df, 0) == results[ri]);
      ++ri;
    }
  }
}

BOOST_AUTO_TEST_CASE(test_hamming_distance_min_01) {
  abn<> code(3, 1, 3, 5);

  abn<> field(3, 1, 3, 1);
  for (int fp(0); fp < 4; ++fp) {
    ++field;
    code.copy_data(fp, field, 0);
  }

  int const results[] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                         2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2};
  int ri(0);

  for (abn<> df(3, 1, 3, 1); df.not_exhausted(); ++df) {
    //    std::cout << "HD " << df << " -> " << code << std::endl;
    BOOST_TEST(code.hamming_distance_min(df, 0) == results[ri]);
    ++ri;
  }
}
