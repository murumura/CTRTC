#ifndef STATIC_VEC_HH
#define STATIC_VEC_HH
#include <array>
#include <cassert>
#include <concepts>
namespace RayTracer {

template <typename T, std::size_t Size>
class StaticVector : protected std::array<T, Size> {
  using storage_type = std::array<T, Size>;

 public:
  using iterator = typename storage_type::iterator;
  using const_iterator = typename storage_type::const_iterator;
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = typename storage_type::difference_type;
  using reference = typename storage_type::reference;
  using const_reference = typename storage_type::const_reference;

  static constexpr size_type max_size = Size;

  template <typename InputIt>
  requires(!std::same_as<InputIt, value_type>) constexpr StaticVector(
      const InputIt& first, const InputIt& last) noexcept {
    InputIt it = first;
    while (it != last) {
      push_back(*it);
      it++;
    }
  }

  template <typename... Args>
  requires(std::same_as<Args, value_type>&&...) constexpr StaticVector(
      Args&&... args)
      : storage_type{std::forward<Args>(args)...}, m_size{sizeof...(Args)} {}

  [[nodiscard]] constexpr auto begin() const noexcept {
    return storage_type::begin();
  }

  [[nodiscard]] constexpr auto begin() noexcept {
    return storage_type::begin();
  }

  [[nodiscard]] constexpr auto end() const noexcept {
    return std::next(begin(), static_cast<difference_type>(m_size));
  }

  [[nodiscard]] constexpr auto end() noexcept {
    return std::next(begin(), static_cast<difference_type>(m_size));
  }

  [[nodiscard]] constexpr auto cbegin() const noexcept {
    return storage_type::cbegin();
  }

  [[nodiscard]] constexpr auto cend() const noexcept {
    return std::next(cbegin(), static_cast<difference_type>(m_size));
  }

  [[nodiscard]] constexpr const value_type& operator[](
      const size_type pos) const noexcept {
    return data()[pos];
  }

  [[nodiscard]] constexpr value_type& operator[](const size_type pos) noexcept {
    return data()[pos];
  }

  constexpr void push_back(value_type value) noexcept {
    data()[m_size++] = std::move(value);
  }

  constexpr void pop_back() noexcept { --m_size; }

  [[nodiscard]] constexpr const value_type& back() const noexcept {
    return data()[m_size - 1];
  }

  [[nodiscard]] constexpr value_type& back() noexcept {
    return data()[m_size - 1];
  }

  [[nodiscard]] constexpr size_type capacity() const noexcept {
    return max_size;
  }

  [[nodiscard]] constexpr size_type size() const noexcept { return m_size; }

  [[nodiscard]] constexpr bool empty() const noexcept { return m_size == 0; }

  [[nodiscard]] constexpr const value_type* data() const noexcept {
    return storage_type::data();
  }

  [[nodiscard]] constexpr value_type* data() noexcept {
    return storage_type::data();
  }

 private:
  size_type m_size{0};
};
}  // namespace RayTracer

#endif
