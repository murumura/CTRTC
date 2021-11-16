#ifndef VEC_HH
#define VEC_HH
#include <array>
#include <type_traits>
namespace RayTracer {
    //
    template <std::size_t N, typename T, typename = typename std::enable_if_t<std::is_arithmetic_v<T>, T>>
    class Vec : public std::array<T, N> {
    public:
        /// The (static/fixed) size of the type
        static constexpr std::size_t size() { return N; }

        /// Explicitly casts the Vec to another Vec that may have
        /// a different precision or dimension. Will default construct
        /// any values that cannot be filled (e.g. vec3 -> vec4) or leave
        /// out the last values when the size of vector is shrinked (e.g.
        /// {1, 2, 3} -> {1, 2}).
        template <size_t NewN, typename NewT>
        constexpr explicit operator Vec<NewN, NewT>() const
        {
            auto ret = Vec<NewN, NewT>{};
            for (auto i = 0u; i < std::min(N, NewN); ++i)
                ret[i] = (*this)[i];
            return ret;
        }
    };

    template <typename... Args>
    Vec(Args&&... args) -> Vec<sizeof...(Args), std::common_type_t<Args...>>;

    template <size_t N, typename T1, typename T2>
    constexpr Vec<N, T1>& operator+=(Vec<N, T1>& a, const Vec<N, T2>& b)
    {
        for (std::size_t i = 0; i < N; ++i)
            a[i] += b[i];
        return a;
    }

    template <size_t N, typename T1, typename T2>
    constexpr auto operator+(const Vec<N, T1>& a, const Vec<N, T2>& b)
    {
        Vec<N, decltype(a[0] + b[0])> ret{};
        for (auto i = 0u; i < N; ++i)
            ret[i] = a[i] + b[i];
        return ret;
    }

    template <size_t N, typename T1, typename T2>
    constexpr Vec<N, T1>& operator-=(Vec<N, T1>& a, const Vec<N, T2>& b)
    {
        for (std::size_t i = 0; i < N; ++i)
            a[i] -= b[i];
        return a;
    }

    template <size_t N, typename T1, typename T2>
    constexpr auto operator-(const Vec<N, T1>& a, const Vec<N, T2>& b)
    {
        Vec<N, decltype(a[0] + b[0])> ret{};
        for (auto i = 0u; i < N; ++i)
            ret[i] = a[i] - b[i];
        return ret;
    }

} // namespace RayTracer

#endif