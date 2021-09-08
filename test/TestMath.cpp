/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2014) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#include <gtest/gtest.h>
#include <Kokkos_Core.hpp>
#include <simd.hpp>

namespace Test {

template <typename ScalarType>
using simd_t =    simd::simd<ScalarType, simd::simd_abi::native>;
//using mask_t = simd_t<double>::mask_type;

// Methods to create data
template<class ValueType>
Kokkos::View<simd::simd<ValueType, simd::simd_abi::native> *> create_data_with_uniq_value(std::string name, int size, ValueType value) {
    Kokkos::View<simd::simd<ValueType, simd::simd_abi::native> *> data(name, size);
    Kokkos::deep_copy(data, simd::simd<ValueType, simd::simd_abi::native>(value));
    return data;
}

template<class ValueType>
Kokkos::View<simd::simd<ValueType, simd::simd_abi::native> *> create_data_zero_to_size_positive(std::string name, int size) {
    Kokkos::View<simd::simd<ValueType, simd::simd_abi::native> *> data(name, size);
    auto data_h = Kokkos::create_mirror_view(Kokkos::HostSpace(), data); // this lives on host
    for(size_t i = 0; i < data_h.extent(0); ++i)
    {
      data_h(i) = simd::simd<ValueType, simd::simd_abi::native>{static_cast<ValueType>(i)};
    }
    Kokkos::deep_copy(data, data_h);

    return data;
}

template<class ValueType>
Kokkos::View<simd::simd<ValueType, simd::simd_abi::native> *> create_data_sqrt(std::string name, int size) {
    Kokkos::View<simd::simd<ValueType, simd::simd_abi::native> *> data(name, size);
    auto data_h = Kokkos::create_mirror_view(Kokkos::HostSpace(), data); // this lives on host
    for(size_t i = 0; i < data_h.extent(0); ++i)
    {
      data_h(i) = simd::simd<ValueType, simd::simd_abi::native>{static_cast<ValueType>(i)} * simd::simd<ValueType, simd::simd_abi::native>{static_cast<ValueType>(i)};
    }
    Kokkos::deep_copy(data, data_h);

    return data;
}

template<class ValueType>
Kokkos::View<simd::simd<ValueType, simd::simd_abi::native> *> create_data_cbrt(std::string name, int size) {
    Kokkos::View<simd::simd<ValueType, simd::simd_abi::native> *> data(name, size);
    auto data_h = Kokkos::create_mirror_view(Kokkos::HostSpace(), data); // this lives on host
    for(size_t i = 0; i < data_h.extent(0); ++i)
    {
      data_h(i) = simd::simd<ValueType, simd::simd_abi::native>{static_cast<ValueType>(i)} * simd::simd<ValueType, simd::simd_abi::native>{static_cast<ValueType>(i)} * simd::simd<ValueType, simd::simd_abi::native>{static_cast<ValueType>(i)};
    }
    Kokkos::deep_copy(data, data_h);

    return data;
}

template<class ValueType>
Kokkos::View<simd_t<ValueType> *> create_data_zero_to_size_negative(std::string name, int size) {
    Kokkos::View<simd_t<ValueType> *> data(name, size);
    auto data_h = Kokkos::create_mirror_view(Kokkos::HostSpace(), data); // this lives on host
    for(size_t i = 0; i < data_h.extent(0); ++i)
    {
      data_h(i) = -simd_t<ValueType>{static_cast<ValueType>(i)};
    }
    Kokkos::deep_copy(data, data_h);

    return data;
}

// Methods for testing
template<class ScalarType>
void test_abs(Kokkos::View<simd_t<ScalarType> *> data) {
  Kokkos::parallel_for(
      data.extent(0),
      KOKKOS_LAMBDA(const int i) { data(i) = simd::abs(data(i)); });
}

template<class ScalarType>
void test_sqrt(Kokkos::View<simd_t<ScalarType> *> data) {
  Kokkos::parallel_for(
      data.extent(0),
      KOKKOS_LAMBDA(const int i) { data(i) = simd::sqrt(data(i)); });
}

template<class ScalarType>
void test_cbrt(Kokkos::View<simd_t<ScalarType> *> data) {
  Kokkos::parallel_for(
      data.extent(0),
      KOKKOS_LAMBDA(const int i) { data(i) = simd::cbrt(data(i)); });
}

template<class ScalarType>
void test_exp(Kokkos::View<simd_t<ScalarType> *> data) {
  Kokkos::parallel_for(
      data.extent(0),
      KOKKOS_LAMBDA(const int i) { data(i) = simd::exp(data(i)); });
}

template<class ScalarType>
void test_fma(Kokkos::View<simd_t<ScalarType> *> data, simd_t<ScalarType> val_1, simd_t<ScalarType> val_2) {
  Kokkos::parallel_for(
      data.extent(0), KOKKOS_LAMBDA(const int i) {
        data(i) = simd::fma(data(i), val_1, val_2);
      });
}

template<typename ScalarType>
void test_max(Kokkos::View<simd::simd<ScalarType, simd::simd_abi::native> *> data, simd::simd<ScalarType, simd::simd_abi::native> val) {
  Kokkos::parallel_for(
      data.extent(0),
      KOKKOS_LAMBDA(const int i) { data(i) = simd::max(data(i), val); });
}

template<class ScalarType>
void test_min(Kokkos::View<simd_t<ScalarType> *> data, simd_t<ScalarType> val) {
  Kokkos::parallel_for(
      data.extent(0),
      KOKKOS_LAMBDA(const int i) { data(i) = simd::min(data(i), val); });
}

template<class ScalarType>
void test_copysign(Kokkos::View<simd_t<ScalarType> *> data, simd_t<ScalarType> sng) {
  Kokkos::parallel_for(
      data.extent(0), KOKKOS_LAMBDA(const int i) {
        data(i) = simd::copysign(data(i), sng);
      });
}

template<class ScalarType>
void test_multiplysign(Kokkos::View<simd_t<ScalarType> *> data, simd_t<ScalarType> sng) {
  Kokkos::parallel_for(
      data.extent(0), KOKKOS_LAMBDA(const int i) {
        data(i) = simd::multiplysign(data(i), sng);
      });
}

template<class ScalarType>
void test_op_add(Kokkos::View<simd_t<ScalarType> *> data, simd_t<ScalarType> val) {
  Kokkos::parallel_for(
      data.extent(0), KOKKOS_LAMBDA(const int i) {
        data(i) = data(i) + val;
      });
}

template<class ScalarType>
void test_op_sub(Kokkos::View<simd_t<ScalarType> *> data, simd_t<ScalarType> val) {
  Kokkos::parallel_for(
      data.extent(0), KOKKOS_LAMBDA(const int i) {
        data(i) = data(i) - val;
      });
}

template<class ScalarType>
void test_op_mul(Kokkos::View<simd_t<ScalarType> *> data, simd_t<ScalarType> val) {
  Kokkos::parallel_for(
      data.extent(0), KOKKOS_LAMBDA(const int i) {
        data(i) = data(i) * val;
      });
}

template<class ScalarType>
void test_op_div(Kokkos::View<simd_t<ScalarType> *> data, simd_t<ScalarType> val) {
  Kokkos::parallel_for(
      data.extent(0), KOKKOS_LAMBDA(const int i) {
        data(i) = data(i) / val;
      });
}

template<typename ScalarType>
void test_view_result(const std::string &test_name, Kokkos::View<simd::simd<ScalarType, simd::simd_abi::native> *> data,
                      ScalarType expected) {
  auto data_h = Kokkos::create_mirror_view(data);
  Kokkos::deep_copy(data_h, data);


  Kokkos::View<ScalarType *, Kokkos::HostSpace> scalar_view(
      reinterpret_cast<ScalarType *>(data_h.data()),
      data_h.extent(0) * simd::simd<ScalarType, simd::simd_abi::native>::size());

  for (size_t i = 0; i < scalar_view.extent(0); ++i) {
    EXPECT_DOUBLE_EQ(scalar_view(i), static_cast<ScalarType>(expected))
        << "Failure during " + test_name + " with i = " + std::to_string(i);
  }
}

template<typename ScalarType>
void test_view_result(const std::string &test_name, Kokkos::View<simd::simd<ScalarType, simd::simd_abi::native> *> data,
                      ScalarType *expected) {
  auto data_h = Kokkos::create_mirror_view(data);
  Kokkos::deep_copy(data_h, data);

  Kokkos::View<ScalarType *, Kokkos::HostSpace> scalar_view(
      reinterpret_cast<ScalarType *>(data_h.data()),
      data_h.extent(0) * simd::simd<ScalarType, simd::simd_abi::native>::size());

  for (size_t i = 0; i < scalar_view.extent(0); ++i) {
    EXPECT_DOUBLE_EQ(scalar_view(i), static_cast<ScalarType>(expected[i]))
        << "Failure during " + test_name + " with i = " + std::to_string(i);
  }
}

template<typename ScalarType>
void do_test_abs() {
    const int simdSize = simd::simd<ScalarType, simd::simd_abi::native>::size();
    const int viewExtent = 8;
    const int expectedSize = viewExtent *  simd::simd<ScalarType, simd::simd_abi::native>::size();

    auto data = create_data_with_uniq_value<ScalarType>("Test View", viewExtent, -4);

    test_abs(data);
    test_view_result("test_abs_1", data, static_cast<ScalarType>(4.0));

    data = create_data_zero_to_size_positive<ScalarType>("Test View 2", 8);


    ScalarType expectedData[expectedSize];
    for(int i = 0; i < viewExtent; ++i) {

        for(int j = 0; j < simdSize; ++j) {
            expectedData[i * simdSize + j] = static_cast<ScalarType>(i);
        }
    }

    test_abs(data);
    test_view_result("test_abs_2", data, expectedData);

    data = create_data_zero_to_size_negative<ScalarType>("Test View 3", 8);

    test_abs(data);
    test_view_result("test_abs_3", data, expectedData);
}

template<typename ScalarType>
void do_test_sqrt() {
    const int simdSize = simd::simd<ScalarType, simd::simd_abi::native>::size();
    const int viewExtent = 8;
    const int expectedSize = viewExtent *  simd::simd<ScalarType, simd::simd_abi::native>::size();

    auto data = create_data_with_uniq_value<ScalarType>("Test View", viewExtent, 16.0);

    test_sqrt(data);
    test_view_result("test_sqrt_1", data, static_cast<ScalarType>(4.0));

    data = create_data_sqrt<ScalarType>("Test View 2", 8);
    ScalarType expectedData[expectedSize];
    for(int i = 0; i < viewExtent; ++i) {

        for(int j = 0; j < simdSize; ++j) {
            expectedData[i * simdSize + j] = static_cast<ScalarType>(i);
        }
    }

    test_sqrt(data);
    test_view_result("test_sqrt_2", data, expectedData);
}

template<typename ScalarType>
void do_test_cbrt() {
    const int simdSize = simd::simd<ScalarType, simd::simd_abi::native>::size();
    const int viewExtent = 8;
    const int expectedSize = viewExtent *  simd::simd<ScalarType, simd::simd_abi::native>::size();

    auto data = create_data_with_uniq_value<ScalarType>("Test View", viewExtent, 27.0);

    test_cbrt(data);
    test_view_result("test_cbrt_1", data, static_cast<ScalarType>(3.0));

    data = create_data_cbrt<ScalarType>("Test View 2", 8);
    ScalarType expectedData[expectedSize];
    for(int i = 0; i < viewExtent; ++i) {

        for(int j = 0; j < simdSize; ++j) {
            expectedData[i * simdSize + j] = static_cast<ScalarType>(i);
        }
    }

    test_cbrt(data);
    test_view_result("test_cbrt_2", data, expectedData);
}

template<typename ScalarType>
void do_test_exp() {
    const int simdSize = simd::simd<ScalarType, simd::simd_abi::native>::size();
    const int viewExtent = 8;
    const int expectedSize = viewExtent *  simd::simd<ScalarType, simd::simd_abi::native>::size();

    auto data = create_data_with_uniq_value<ScalarType>("Test View", viewExtent, 1.0);

    test_exp(data);
    test_view_result("test_exp_1", data, static_cast<ScalarType>(std::exp(1.0)));

    data = create_data_zero_to_size_positive<ScalarType>("Test View 2", viewExtent);
    ScalarType expectedData[expectedSize];
    for(int i = 0; i < viewExtent; ++i) {

        for(int j = 0; j < simdSize; ++j) {
            expectedData[i * simdSize + j] = static_cast<ScalarType>(std::exp(i));
        }
    }

    test_exp(data);
    test_view_result("test_exp_2", data, expectedData);
}

template<typename ScalarType>
void do_test_fma() {
    const int simdSize = simd::simd<ScalarType, simd::simd_abi::native>::size();
    const int viewExtent = 8;
    const int expectedSize = viewExtent *  simd::simd<ScalarType, simd::simd_abi::native>::size();

    auto data = create_data_with_uniq_value<ScalarType>("Test View", viewExtent, -4.0);

    test_fma(data, simd_t<ScalarType>{2.0}, simd_t<ScalarType>{5.0});
    test_view_result("test_fma_1", data, static_cast<ScalarType>(-3.0));

    data = create_data_zero_to_size_positive<ScalarType>("Test View 2", viewExtent);
    ScalarType expectedData[expectedSize];
    for(int i = 0; i < viewExtent; ++i) {

        for(int j = 0; j < simdSize; ++j) {
            expectedData[i * simdSize + j] = static_cast<ScalarType>(7.0 + 4*i);
        }
    }

    test_fma(data, simd_t<ScalarType>{4.0}, simd_t<ScalarType>{7.0});
    test_view_result("test_fma_2", data, expectedData);

    data = create_data_zero_to_size_negative<ScalarType>("Test View 2", viewExtent);
    for(int i = 0; i < viewExtent; ++i) {

        for(int j = 0; j < simdSize; ++j) {
            expectedData[i * simdSize + j] = static_cast<ScalarType>(7.0 - 4*i);
        }
    }

    test_fma(data, simd_t<ScalarType>{4.0}, simd_t<ScalarType>{7.0});
    test_view_result("test_fma_3", data, expectedData);
}

template<typename ScalarType>
void do_test_max() {
    const int simdSize = simd::simd<ScalarType, simd::simd_abi::native>::size();
    const int viewExtent = 8;
    const int expectedSize = viewExtent *  simd::simd<ScalarType, simd::simd_abi::native>::size();

    auto data = create_data_with_uniq_value<ScalarType>("Test View", viewExtent, 1.0);

    test_max(data, simd_t<ScalarType>{10.0});
    test_view_result("test_max_1", data, static_cast<ScalarType>(10.0));

    data = create_data_sqrt<ScalarType>("Test View 2", viewExtent);
    ScalarType expectedData[expectedSize];
    for(int i = 0; i < viewExtent; ++i) {

        for(int j = 0; j < simdSize; ++j) {
            if( i < 4) {
                expectedData[i * simdSize + j] = static_cast<ScalarType>(10.0);
            } else {
                expectedData[i * simdSize + j] = static_cast<ScalarType>(i * i);
            }
        }
    }

    test_max(data, simd_t<ScalarType>{10.0});
    test_view_result("test_max_2", data, expectedData);
}

template<typename ScalarType>
void do_test_min() {
    const int simdSize = simd::simd<ScalarType, simd::simd_abi::native>::size();
    const int viewExtent = 8;
    const int expectedSize = viewExtent *  simd::simd<ScalarType, simd::simd_abi::native>::size();

    auto data = create_data_with_uniq_value<ScalarType>("Test View", viewExtent, 4.0);

    test_min(data, simd_t<ScalarType>{1.0});
    test_view_result("test_min_1", data, static_cast<ScalarType>(1.0));

    data = create_data_sqrt<ScalarType>("Test View 2", viewExtent);
    ScalarType expectedData[expectedSize];
    for(int i = 0; i < viewExtent; ++i) {

        for(int j = 0; j < simdSize; ++j) {
            if( i < 4) {
                expectedData[i * simdSize + j] = static_cast<ScalarType>(i * i);
            } else {
                expectedData[i * simdSize + j] = static_cast<ScalarType>(10.0);
            }
        }
    }

    test_min(data, simd_t<ScalarType>{10.0});
    test_view_result("test_min_2", data, expectedData);
}

TEST(simd_math, test_abs) {
    do_test_abs<double>();
    do_test_abs<float>();
 }

TEST(simd_math, test_sqrt) {
    do_test_sqrt<double>();
    do_test_sqrt<float>();
}

TEST(simd_math, test_cbrt) {
    do_test_cbrt<double>();
    do_test_cbrt<float>();
}

TEST(simd_math, test_exp) {
    do_test_exp<double>();
    do_test_exp<float>();
}

TEST(simd_math, test_fma) {
    do_test_fma<double>();
    do_test_fma<float>();
}

TEST(simd_math, test_max) {
    do_test_max<double>();
    do_test_max<float>();
}

TEST(simd_math, test_min) {
    do_test_min<double>();
    do_test_min<float>();
}

TEST(simd_math, test_op_add) {
  Kokkos::View<simd_t<double> *> data = create_data_with_uniq_value<double>("Test View", 8, 4.0);

  test_op_add(data, simd_t<double>{10.0});
  test_view_result("test_op_add_1", data, 14.0);

  data = create_data_zero_to_size_positive<double>("Test View 2", 8);
  const int expectedSize = 16;
  double expectedData[expectedSize]= {4.0,4.0,5.0,5.0,6.0,6.0,7.0,7.0,8.0,8.0,9.0,9.0,10.0,10.0,11.0,11.0};

  test_op_add(data, simd_t<double>{4.0});
  test_view_result("test_op_add_2", data, expectedData);
}

TEST(simd_math, test_op_sub) {
  Kokkos::View<simd_t<double> *> data = create_data_with_uniq_value<double>("Test View", 8, 4.0);

  test_op_sub(data, simd_t<double>{10.0});
  test_view_result("test_op_sub_1", data, -6.0);

  data = create_data_zero_to_size_positive<double>("Test View 2", 8);
  const int expectedSize = 16;
  double expectedData[expectedSize]= {-4.0,-4.0,-3.0,-3.0,-2.0,-2.0,-1.0,-1.0,0.0,0.0,1.0,1.0,2.0,2.0,3.0,3.0};

  test_op_sub(data, simd_t<double>{4.0});
  test_view_result("test_op_sub_1", data, expectedData);
}

TEST(simd_math, test_op_mul) {
  Kokkos::View<simd_t<double> *> data = create_data_with_uniq_value<double>("Test View", 8, 4.0);

  test_op_mul(data, simd_t<double>{10.0});
  test_view_result("test_op_mul_1", data, 40.0);

  data = create_data_zero_to_size_positive<double>("Test View 2", 8);
  const int expectedSize = 16;
  double expectedData[expectedSize]= {0.0,0.0,4.0,4.0,8.0,8.0,12.0,12.0,16.0,16.0,20.0,20.0,24.0,24.0,28.0,28.0};

  test_op_mul(data, simd_t<double>{4.0});
  test_view_result("test_op_mul_2", data, expectedData);
}

TEST(simd_math, test_op_div) {
  Kokkos::View<simd_t<double> *> data = create_data_with_uniq_value<double>("Test View", 8, 4.0);

  test_op_div(data, simd_t<double>{10.0});
  test_view_result("test_op_div_1", data, 0.4);

  data = create_data_zero_to_size_positive<double>("Test View 2", 8);
  const int expectedSize = 16;
  double expectedData[expectedSize]= {0.0,0.0,0.25,0.25,0.5,0.5,0.75,0.75,1.0,1.0,1.25,1.25,1.50,1.50,1.75,1.75};

  test_op_div(data, simd_t<double>{4.0});
  test_view_result("test_op_div_2", data, expectedData);
}

TEST(simd_math, test_copysign) {
    Kokkos::View<simd_t<double> *> data = create_data_zero_to_size_positive<double>("Test View", 8);

    const int expectedSize = 16; // 8 * 2
    test_copysign(data, simd_t<double>{2.0});
    double expectedData[expectedSize]= {0.0,0.0,1.0,1.0,2.0,2.0,3.0,3.0,4.0,4.0,5.0,5.0,6.0,6.0,7.0,7.0};
    test_view_result("test_copysign_1", data, expectedData);

    test_copysign(data, simd_t<double>{-2.0});
    for(int i = 0; i < expectedSize; ++i) {
       expectedData[i] = -expectedData[i];
    }
    test_view_result("test_copysign_2", data, expectedData);

    test_copysign(data, simd_t<double>{4.0});
    for(int i = 0; i < expectedSize; ++i) {
       expectedData[i] = -expectedData[i];
    }
    test_view_result("test_copysign_3", data, expectedData);
}

TEST(simd_math, test_multiplysign) {
    Kokkos::View<simd_t<double> *> data("Test View", 8);
    Kokkos::deep_copy(data, simd_t<double>(4.0));

    test_multiplysign(data, simd_t<double>{2.0});
    test_view_result("test_multiplysign_1", data, 4.0);

    test_multiplysign(data, simd_t<double>{-2.0});
    test_view_result("test_multiplysign_2", data, -4.0);

    test_multiplysign(data, simd_t<double>{2.0});
    test_view_result("test_multiplysign_3", data, -4.0);

    data = create_data_zero_to_size_positive<double>("Test View", 8);
    const int expectedSize = 16; // 8 * 2
    test_multiplysign(data, simd_t<double>{2.0});
    double expectedData[expectedSize]= {0.0,0.0,1.0,1.0,2.0,2.0,3.0,3.0,4.0,4.0,5.0,5.0,6.0,6.0,7.0,7.0};
    test_view_result("test_multiplysign_4", data, expectedData);

    test_multiplysign(data, simd_t<double>{-2.0});
    for(int i = 0; i < expectedSize; ++i) {
       expectedData[i] = -expectedData[i];
    }
    test_view_result("test_multiplysign_5", data, expectedData);

    test_multiplysign(data, simd_t<double>{-4.0});
    for(int i = 0; i < expectedSize; ++i) {
       expectedData[i] = -expectedData[i];
    }
    test_view_result("test_multiplysign_6", data, expectedData);

    test_multiplysign(data, simd_t<double>{4.0});
    test_view_result("test_multiplysign_7", data, expectedData);
}

}  // namespace Test
