#ifndef PRITRAITS_HH
#define PRITRAITS_HH
#include <concepts>
#include <type_traits>
namespace RayTracer {
namespace PrimitiveTraits {

template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template <typename Ret, typename F, typename... Args>
concept InvocableWithRetType = std::is_invocable_r_v<Ret, F, Args...>;

template <typename T, T... v>
struct AllSameValue {};

template <typename T, T v1, T v2, T... rest>
struct AllSameValue<T, v1, v2, rest...> : AllSameValue<T, v2, rest...> {
  static_assert(v1 == v2,
                "All values in the template parameter list must be equal");
  static constexpr T value = v1;
};

template <typename T, T v>
struct AllSameValue<T, v> {
  static constexpr T value = v;
};

template <typename T, T... v>
constexpr auto AllSameValue_v = AllSameValue<T, v...>::value;

template <typename T, typename... Args>
struct Contains : public std::bool_constant<(std::is_same<T, Args>{} || ...)> {
};

template <typename T>
using RemoveCVR = std::remove_cv_t<std::remove_reference_t<T>>;

}  // namespace PrimitiveTraits
}  // namespace RayTracer

#endif
