#ifndef PRITRAITS_HH
#define PRITRAITS_HH
namespace RayTracer {
    namespace PrimitiveTraits {
        template <typename T, T... v>
        struct AllSameValue {
        };

        template <typename T, T v1, T v2, T... rest>
        struct AllSameValue<T, v1, v2, rest...> : AllSameValue<T, v2, rest...> {
            static_assert(v1 == v2, "All values in the template parameter list must be equal");
            static constexpr T value = v1;
        };

        template <typename T, T v>
        struct AllSameValue<T, v> {
            static constexpr T value = v;
        };

        template <typename T, T... v>
        constexpr auto AllSameValue_v = AllSameValue<T, v...>::value;

    } // namespace PrimitiveTraits
} // namespace RayTracer

#endif
