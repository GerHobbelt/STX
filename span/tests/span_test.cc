/**
 * @file span_test.cc
 * @author Basit Ayantunde <rlamarrr@gmail.com>
 * @date 2020-08-04
 *
 * @copyright MIT License
 *
 * Copyright (c) 2020-2021 Basit Ayantunde
 *
 */

#include "stx/span.h"

#include <array>
#include <utility>

#include "gtest/gtest.h"

using namespace std;
using namespace string_literals;
using namespace stx;

static_assert(impl::is_span_convertible<int, volatile int>);

TEST(SpanTest, CopyConstructor) {
  vector<int> vec{1, 2, 3, 4, 5};

  {
    Span a = Span<int>(vec.data(), vec.size());

    Span<volatile int> b{a};

    EXPECT_EQ(a.data(), vec.data());
    EXPECT_EQ(a.data(), b.data());

    EXPECT_EQ(a.size(), b.size());
  }
}

TEST(SpanTest, ConstructorCArray) {
  int tmp[] = {1, 2, 3, 4};

  {
    Span<int> a = tmp;

    EXPECT_EQ(a.data(), tmp);
    EXPECT_EQ(a.size(), size(tmp));
  }
}

TEST(SpanTest, ConstructorStdArray) {
  array<int, 4> tmp{1, 2, 3, 4};
  {
    Span<int> a = tmp;

    EXPECT_EQ(a.data(), tmp.data());
    EXPECT_EQ(a.size(), size(tmp));
  }
}

TEST(SpanTest, Empty) {
  {
    Span<int> a{};
    EXPECT_TRUE(a.is_empty());
  }

  {
    Span<int> a(nullptr, 0);
    EXPECT_TRUE(a.is_empty());
  }
}

TEST(SpanTest, At) {
  int tmp[] = {1, 2, 3, 4};
  {
    Span<int> a(tmp, size(tmp));

    EXPECT_EQ(a.at(4), None);
    EXPECT_EQ(a.at(3), Some(4));
  }
}

TEST(SpanTest, As) {
  {
    int32_t tmp[] = {1, 2, 3, 4};
    Span<int32_t> a(tmp);
    Span<int32_t const> tmp_a = a.as_const();
    Span<byte const> tmp_b = a.as_bytes();
    Span<uint8_t const> tmp_c = a.as_u8();
    Span<volatile int32_t> tmp_d = a.as_volatile();

    Span<byte const volatile> b = a.as_u8().as_volatile().as_const().as_bytes();

    EXPECT_EQ((void *)a.data(), (void *)b.data());
    EXPECT_EQ(a.size_bytes(), b.size());
    EXPECT_EQ(a.size() * 4, b.size());
  }
}

struct result_t {
  size_t size;
  int value;
};

namespace t1 {
constexpr result_t Span_Slice(size_t index) {
  int tmp[] = {1, 2, 3, 4};

  Span<int> h = tmp;

  return {h.slice(index).size(), h.slice(index)[0]};
}

}  // namespace t1

TEST(SpanTest, Slice) {
  using namespace t1;
  {
    constexpr auto a = Span_Slice(0);
    EXPECT_EQ(a.size, 4);
    EXPECT_EQ(a.value, 1);
  }

  {
    constexpr auto a = Span_Slice(3);
    EXPECT_EQ(a.size, 1);
    EXPECT_EQ(a.value, 4);
  }
}

TEST(SpanTest, Algorithms) {
  int y[] = {1, 2, 3, 4, 5, 6};

  Span<int> r{y};
  Span<int> e{};

  r.set(8);

  for (auto &u : r) {
    std::cout << u << std::endl;
  }

  EXPECT_TRUE(r.all_equals(8));
  EXPECT_FALSE(r.none_equals(8));

  EXPECT_TRUE(e.is_empty());
  EXPECT_FALSE(r.all_equals(0));
  EXPECT_FALSE(r.any_equals(0));
  EXPECT_TRUE(r.none_equals(0));

  EXPECT_TRUE(r.map([](int a) { return a + 1; }, r).all_equals(9));
  r.find(9).set(64);
  EXPECT_EQ(r[0], 64);

  for (int &element : r.slice(1)) {
    EXPECT_EQ(element, 9);
  }

  {
    int g[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    Span<int> f = g;

    auto [a, b] = f.partition([](int x) { return x < 5; });

    EXPECT_EQ(a.size(), 4);
    EXPECT_EQ(b.size(), 6);

    auto [c, d] = b.partition([](int x) { return x > 8; });

    EXPECT_EQ(c.size(), 2);
    EXPECT_EQ(d.size(), 4);
  }
}
