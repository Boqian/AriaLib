#pragma once

namespace aria {

template <class T, class E> class expected {
public:
  using value_type = T;
  using error_type = E;
  template <class U> using rebind = expected<U, error_type>;

private:
  bool m_has_value;
  union {
    T m_val;
    E m_err;
  };
};

} // namespace aria