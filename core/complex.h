#pragma once
#include <cmath>

namespace aria {

template <class T> class complex {
public:
  using value_type = T;
  constexpr complex(const T &re = T(), const T &im = T()) : x(re), y(im) {}

  constexpr T real() const { return x; }
  constexpr T imag() const { return y; }
  constexpr void real(T a) { x = a; }
  constexpr void imag(T a) { y = a; }

  constexpr complex &operator+=(const complex &rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }
  constexpr complex &operator-=(const complex &rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }
  constexpr complex &operator*=(const complex &rhs) {
    const auto a = x * rhs.x - y * rhs.y, b = rhs.x * y + x * rhs.y;
    x = a, y = b;
    return *this;
  }
  constexpr complex &operator/=(const complex &rhs) {
    const auto d2 = rhs.x * rhs.x + rhs.y * rhs.y, a = x * rhs.x + y * rhs.y, b = rhs.x * y - x * rhs.y;
    x = a / d2, y = b / d2;
    return *this;
  }

private:
  T x;
  T y;
};

template <class T> constexpr complex<T> operator+(const complex<T> &a) { return a; }
template <class T> constexpr complex<T> operator-(const complex<T> &a) { return complex<T>(-a.real(), -a.imag()); }
template <class T> constexpr complex<T> operator+(const complex<T> &a, const complex<T> &b) noexcept { return complex<T>(a) += b; }
template <class T> constexpr complex<T> operator-(const complex<T> &a, const complex<T> &b) noexcept { return complex<T>(a) -= b; }
template <class T> constexpr complex<T> operator*(const complex<T> &a, const complex<T> &b) noexcept { return complex<T>(a) *= b; }
template <class T> constexpr complex<T> operator/(const complex<T> &a, const complex<T> &b) noexcept { return complex<T>(a) /= b; }

template <class T> constexpr T real(const complex<T> &z) { return z.real(); }
template <class T> constexpr T imag(const complex<T> &z) { return z.imag(); }
template <class T> T abs(const complex<T> &z) { return sqrt(z.real() * z.real() + z.imag() * z.imag()); }

} // namespace aria