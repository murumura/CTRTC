#ifndef MATH_HH
#define MATH_HH
#include <numeric>
#include <type_traits>
#include <utility>
#define EPSILON 1e-4
namespace RayTracer {
    namespace MathUtils {
        namespace MathConstants {

            template <typename T = double>
            constexpr T PI = T(3.1415926535897932385L);

            template <typename T = double>
            constexpr T INF = std::numeric_limits<T>::infinity();

            template <typename T = double>
            constexpr T NINF = -std::numeric_limits<T>::infinity();

        }; // namespace MathConstants

        /* Constexpr square root value for floating point value. */
        template <typename T>
        constexpr typename std::enable_if_t<std::is_floating_point_v<T>, T>
        ConstExprSqrtf(T val)
        {
            return std::sqrt(val);
        }

        /* constexpr absolute value of a floating point value. */
        template <typename T>
        constexpr typename std::enable_if_t<std::is_floating_point_v<T>, T>
        ConstExprAbsf(T val)
        {
            return val < static_cast<T>(0) ? -val : val;
        }

        template <typename S, typename T>
        constexpr typename std::enable_if_t<std::is_floating_point_v<S> || std::is_floating_point_v<T>, bool>
        ApproxEqual(S x, T y)
        {
            // We need special treatment for infinities here.
            if (x == std::numeric_limits<S>::infinity() && y == std::numeric_limits<S>::infinity())
                return true;
            if (x == -std::numeric_limits<T>::infinity() && y == -std::numeric_limits<T>::infinity())
                return true;
            return (ConstExprAbsf(x - y) < EPSILON);
        }
    } // namespace MathUtils
} // namespace RayTracer

#endif