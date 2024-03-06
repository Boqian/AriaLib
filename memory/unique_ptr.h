#pragma once
namespace aria {


template <class T, class Deleter = std::default_delete<T>>
class unique_ptr {
public:
using element_type = T;
using pointer = T*;
using deleter_type = Deleter;

  unique_ptr(T* ptr) : m_ptr(ptr) {}

  unique_ptr(T* ptr, Deleter&& deleter)
      : m_ptr(ptr),
        m_deleter(std::forward<Deleter>(deleter)){}

  ~unique_ptr() { reset();
  }

  unique_ptr(const unique_ptr&) = delete;
  unique_ptr& operator=(const& unique_ptr) noexcept = delete;  

  unique_ptr& operaor = (std::nullptr_t) {
    reset();
    return m_ptr;
  }

  unique_ptr(unique_ptr&& ptr) noexcept {

  }

  void reset() {
    if (m_ptr) {
      m_deleter(m_ptr);
      m_ptr = nullptr;
    }
  }

private:
  T* m_ptr;
 Deleter m_deleter;

};

}  // namespace aria