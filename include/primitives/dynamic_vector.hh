#ifndef DYNAMIC_VEC_HH
#define DYNAMIC_VEC_HH
#include <cassert>
#include <iterator>
#include <limits>
#include <memory>
#include <utility>
namespace RayTracer {

template <typename T>
struct DynamicVector {
  using value_type = T;
  using size_type = std::size_t;
  std::allocator<T> alloc_ = {};
  int m_capacity = 0;
  int m_size = 0;
  T* m_data = nullptr;

  constexpr ~DynamicVector() {
    clear();
    deallocate(m_data, m_capacity);
    m_capacity = 0;
  }

  constexpr DynamicVector() = default;

  constexpr DynamicVector(int n)
      : m_capacity(n), m_size(n), m_data(allocate(m_capacity)) {
    assert(n >= 0);
    assert(std::is_default_constructible_v<T> || n == 0);
    for (int i = 0; i < m_size; ++i) {
      construct(i);
    }
  }

  template <std::convertible_to<T>... Ts>
  constexpr DynamicVector(std::in_place_t, Ts&&... ts) {
    reserve(sizeof...(ts));
    (emplace_back(std::forward<Ts>(ts)), ...);
  }

  template <std::convertible_to<T>... Ts>
  constexpr DynamicVector(std::in_place_type_t<T>, Ts&&... ts)
      : DynamicVector(std::in_place, std::forward<Ts>(ts)...) {}

  constexpr DynamicVector(const DynamicVector& b)
      : m_capacity(b.m_capacity),
        m_size(b.m_size),
        m_data(allocate(m_capacity)) {
    // copy construct the storage array elements
    for (int i = 0; i < m_size; ++i) {
      construct(i, b[i]);
    }
  }

  constexpr DynamicVector(DynamicVector&& b)
      : m_capacity(std::exchange(b.m_capacity, 0)),
        m_size(std::exchange(b.m_size, 0)),
        m_data(std::exchange(b.m_data, nullptr)) {}

  constexpr DynamicVector& operator=(const DynamicVector& other) {
    // if `other` has a larger capacity, then clear and reallocate our storage array
    if (m_capacity < other.m_capacity) {
      clear();
      deallocate(m_data, m_capacity);
      m_data = allocate(other.m_capacity);
      m_capacity = other.m_capacity;
    }

    // copy assign or construct as necessary
    int i = 0;
    for (; i < std::min(m_size, other.m_size); ++i) {
      m_data[i] = other[i];
    }
    for (; i < other.m_size; ++i) {
      construct(i, other[i]);
    }
    for (; i < m_size; ++i) {
      destroy(i);
    }
    m_size = other.m_size;
    return *this;
  }

  constexpr DynamicVector& operator=(DynamicVector&& other) {
    // destroy our active storage array
    clear();
    deallocate(m_data, m_capacity);

    // and take all of other's data
    m_capacity = std::exchange(other.m_capacity, 0);
    m_size = std::exchange(other.m_size, 0);
    m_data = std::exchange(other.m_data, nullptr);

    return *this;
  }

  // Comparisons.
  constexpr friend bool operator==(DynamicVector const& lhs,
                                   DynamicVector const& rhs) {
    return lhs.size() == rhs.size() and
           std::equal(lhs.begin(), lhs.end(), rhs.begin());
  }

  // Element access.
  constexpr const T& operator[](const size_type i) const {
    assert(0 <= i && i < m_size);
    return m_data[i];
  }

  constexpr T& operator[](const size_type i) {
    assert(0 <= i && i < m_size);
    return m_data[i];
  }

  constexpr const T& front() const {
    assert(m_size > 0);
    return m_data[0];
  }

  constexpr T& front() {
    assert(m_size > 0);
    return m_data[0];
  }

  constexpr const T& back() const {
    assert(m_size > 0);
    return m_data[m_size - 1];
  }

  constexpr T& back() {
    assert(m_size > 0);
    return m_data[m_size - 1];
  }

  const T* data() const { return m_data; }

  T* data() { return m_data; }

  constexpr const T* begin() const { return m_data; }
  constexpr T* begin() { return m_data; }
  constexpr const T* end() const { return m_data + m_size; }
  constexpr T* end() { return m_data + m_size; }

  constexpr auto rbegin() const { return std::reverse_iterator(end()); }
  constexpr auto rbegin() { return std::reverse_iterator(end()); }
  constexpr auto rend() const { return std::reverse_iterator(begin()); }
  constexpr auto rend() { return std::reverse_iterator(begin()); }

  // Capacity
  constexpr bool empty() const { return m_size == 0; }

  constexpr size_type size() const { return m_size; }

  constexpr static size_type max_size() {
    return std::numeric_limits<decltype(m_capacity)>::max();
  }

  constexpr void reserve(const size_type n) {
    if (m_capacity < n) {
      reallocate(n);
    }
  }

  constexpr size_type capacity() { return m_capacity; }

  constexpr friend size_type size(const DynamicVector& v) { return v.m_size; }

  // Modifiers
  template <typename... Ts>
  constexpr T& emplace_back(Ts&&... ts) {
    static_assert(std::is_constructible_v<T, Ts...>);
    if (m_size == m_capacity) {
      reserve(std::max(2 * m_capacity, 1));
    }
    return construct(m_size++, std::forward<Ts>(ts)...);
  }

  constexpr T& push_back(const T& t) {
    if (m_size == m_capacity) {
      reserve(std::max(2 * m_capacity, 1));
    }
    return construct(m_size++, t);
  }

  constexpr T& push_back(T&& t) {
    if (m_size == m_capacity) {
      reserve(std::max(2 * m_capacity, 1));
    }
    return construct(m_size++, std::move(t));
  }

  constexpr T pop_back() {
    assert(m_size > 0);
    return std::move(m_data[--m_size]);
  }

  constexpr void remove_at(size_type i) {
    assert(0 <= i and i < m_size);
    for (size_type e = --m_size; i < e; ++i) {
      m_data[i] = std::move(m_data[i + 1]);
    }
  }

  constexpr void remove(T* i) { remove_at(std::distance(begin(), i)); }

  constexpr void resize(const size_type n) {
    // reserve enough space for n
    if (m_capacity < n) {
      reserve(n);
    }

    // if n was smaller than the size, destroy the excess elements
    for (size_type i = n; i < m_size; ++i) {
      destroy(i);
    }

    // if n was larger than the size, then default construct the excess
    for (size_type i = m_size; i < n; ++i) {
      assert(std::is_default_constructible_v<T>);
      construct(i);
    }

    m_size = n;
  }

  constexpr void clear() {
    // destroy the active elements
    for (size_type i = 0; i < m_size; ++i) {
      destroy(i);
    }
    m_size = 0;
  }

 private:
  constexpr T* allocate(int n) {
    return (n != 0) ? alloc_.allocate(n) : nullptr;
  }

  constexpr void deallocate(T* ptr, const size_type n) {
    if (ptr) {
      alloc_.deallocate(ptr, n);
    }
  }

  template <class... Ts>
  constexpr T& construct(const size_type i, Ts&&... ts) {
    return *std::construct_at(m_data + i, std::forward<Ts>(ts)...);
  }

  constexpr void destroy(const size_type i) { std::destroy_at(m_data + i); }

  constexpr void reallocate(const size_type n) {
    T* old = std::exchange(m_data, allocate(n));
    for (size_type i = 0; i < m_size; ++i) {
      construct(i, std::move(old[i]));
    }
    if (old) {
      alloc_.deallocate(old, m_capacity);
    }
    m_capacity = n;
  }
};

template <typename T, std::convertible_to<T>... Ts>
DynamicVector(std::in_place_t, T, Ts...) -> DynamicVector<T>;

template <typename T, std::convertible_to<T>... Ts>
DynamicVector(std::in_place_type_t<T>, Ts...) -> DynamicVector<T>;
}  // namespace RayTracer

#endif
