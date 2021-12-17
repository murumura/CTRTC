#ifndef MAT_HH
#define MAT_HH
#include <array>
#include <primitive_traits.hh>
#include <primitives/vec.hh>
#include <type_traits>
#include <utils/math.hh>

namespace RayTracer {

    template <typename T, std::size_t R, std::size_t C, typename = typename std::enable_if_t<std::is_arithmetic_v<T>, T>>
    class Matrix final {
    public:
        static_assert(C > 0 && R > 0);
        static constexpr std::size_t Rows = R;
        static constexpr std::size_t Cols = C;
        using ValueType = T;
        constexpr Vec<T, Cols> Row(std::size_t rowIndex) const
        {
            if (rowIndex >= Rows)
                throw "index out of range";
            return VecUtils::Generate<Cols>([rowIndex, this](std::size_t j) { return contents[rowIndex][j]; });
        }

        constexpr Vec<T, Rows> Column(std::size_t colIndex) const
        {
            if (colIndex >= Cols)
                throw "index out of range";
            return VecUtils::Generate<Rows>([colIndex, this](std::size_t j) { return contents[j][colIndex]; });
        }

        /* Returns a pointer to the specified row 
         * The intention of this function is to then access the specified element from the
         * row pointer.
        */
        constexpr T* operator[](std::size_t rowIndex) { return contents[rowIndex]; }

        /* Returns a pointer to the specified row 
         * The intention of this function is to then access the specified element from the
         * row pointer.
        */
        constexpr T const* operator[](std::size_t rowIndex) const { return contents[rowIndex]; }
        T contents[Rows][Cols];
    };

    template <typename T, std::size_t R, std::size_t C>
    Matrix(const T (&)[R][C]) -> Matrix<T, R, C>;

    namespace MatrixUtils {
        template <std::size_t R, std::size_t C, typename Func>
        constexpr decltype(auto) Generate(Func&& func)
        {
            Matrix<std::invoke_result_t<Func, std::size_t, std::size_t>, R, C> generated = {};
            for (std::size_t i = 0; i < R; ++i) {
                for (std::size_t j = 0; j < C; ++j) {
                    generated[i][j] = std::apply(func, std::forward_as_tuple(i, j));
                }
            }
            return generated;
        }

        template <
            typename Func, typename T, typename... Matrices,
            typename ResultType = std::invoke_result_t<Func, T, typename Matrices::ValueType...>,
            std::size_t C = PrimitiveTraits::AllSameValue_v<std::size_t, Matrices::Cols...>,
            std::size_t R = PrimitiveTraits::AllSameValue_v<std::size_t, Matrices::Rows...>>
        constexpr Matrix<T, R, C> ElementWise(Func func, const Matrix<T, R, C>& mat, const Matrices&... matrices)
        {
            Matrix<ResultType, R, C> opApplied = {};
            for (std::size_t i = 0; i < R; ++i) {
                for (std::size_t j = 0; j < C; ++j) {
                    opApplied[i][j] = std::apply(func, std::forward_as_tuple(mat[i][j], matrices[i][j]...));
                }
            }
            return opApplied;
        }

    } // namespace MatrixUtils

    template <typename T, std::size_t R, std::size_t C>
    constexpr bool operator==(const Matrix<T, R, C>& matA, const Matrix<T, R, C>& matB)
    {
        for (std::size_t i = 0; i < R; ++i) {
            for (std::size_t j = 0; j < C; ++j) {
                if (!MathUtils::ApproxEqual(matA[i][j], matB[i][j]))
                    return false;
            }
        }
        return true;
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr bool operator!=(const Matrix<T, R, C>& matA, const Matrix<T, R, C>& matB)
    {
        return !(matA == matB);
    }

    /** Numerical negative, element-wise. */
    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator-(const Matrix<T, R, C>& mat)
    {
        return MatrixUtils::ElementWise([](T x) { return -1.0 * x; }, mat);
    }

    /* Element-wise add */
    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator+(const Matrix<T, R, C>& mat, T scalar)
    {
        return MatrixUtils::ElementWise([scalar](T x) { return x + scalar; }, mat);
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator+(T scalar, const Matrix<T, R, C>& mat)
    {
        return MatrixUtils::ElementWise([scalar](T x) { return x + scalar; }, mat);
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator+(const Matrix<T, R, C>& matA, const Matrix<T, R, C>& matB)
    {
        return MatrixUtils::ElementWise(std::plus<T>(), matA, matB);
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C>& operator+=(Matrix<T, R, C>& mat, T scalar) noexcept
    {
        for (std::size_t row = 0; row < R; row++) {
            for (std::size_t col = 0; col < C; col++) {
                mat[row][col] += scalar;
            }
        }
        return mat;
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C>& operator+=(Matrix<T, R, C>& matA, const Matrix<T, R, C>& matB) noexcept
    {
        for (std::size_t row = 0; row < R; row++) {
            for (std::size_t col = 0; col < C; col++) {
                matA[row][col] += matB[row][col];
            }
        }
        return matA;
    }

    /* Element-wise negation */
    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator-(const Matrix<T, R, C>& mat, T scalar)
    {
        return MatrixUtils::ElementWise([scalar](T x) { return x - scalar; }, mat);
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator-(const Matrix<T, R, C>& matA, const Matrix<T, R, C>& matB)
    {
        return MatrixUtils::ElementWise(std::minus<T>(), matA, matB);
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C>& operator-=(Matrix<T, R, C>& mat, T scalar) noexcept
    {
        for (std::size_t row = 0; row < R; row++) {
            for (std::size_t col = 0; col < C; col++) {
                mat[row][col] -= scalar;
            }
        }
        return mat;
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C>& operator-=(Matrix<T, R, C>& matA, const Matrix<T, R, C>& matB) noexcept
    {
        for (std::size_t row = 0; row < R; row++) {
            for (std::size_t col = 0; col < C; col++) {
                matA[row][col] -= matB[row][col];
            }
        }
        return matA;
    }

    /* Element-wise mulplication */
    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator*(const Matrix<T, R, C>& mat, T scalar)
    {
        return MatrixUtils::ElementWise([scalar](T x) { return scalar * x; }, mat);
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator*(T scalar, const Matrix<T, R, C>& mat)
    {
        return MatrixUtils::ElementWise([scalar](T x) { return scalar * x; }, mat);
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator*(const Matrix<T, R, C>& matA, const Matrix<T, R, C>& matB)
    {
        return MatrixUtils::ElementWise(std::multiplies<T>(), matA, matB);
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C>& operator*=(const Matrix<T, R, C>& matA, const Matrix<T, R, C>& matB) noexcept
    {
        for (std::size_t row = 0; row < R; row++) {
            for (std::size_t col = 0; col < C; col++) {
                matA[row][col] *= matB[row][col];
            }
        }
        return matA;
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C>& operator*=(Matrix<T, R, C>& mat, T scalar) noexcept
    {
        for (std::size_t row = 0; row < R; row++) {
            for (std::size_t col = 0; col < C; col++) {
                mat[row][col] *= scalar;
            }
        }
        return mat;
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C>& operator*=(T scalar, Matrix<T, R, C>& mat) noexcept
    {
        for (std::size_t row = 0; row < R; row++) {
            for (std::size_t col = 0; col < C; col++) {
                mat[row][col] *= scalar;
            }
        }
        return mat;
    }

    /* Element-wise division */
    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator/(const Matrix<T, R, C>& mat, T scalar)
    {
        return MatrixUtils::ElementWise([scalar](T x) { return x / scalar; }, mat);
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator/(const Matrix<T, R, C>& matA, const Matrix<T, R, C>& matB)
    {
        return MatrixUtils::ElementWise(std::divides<T>(), matA, matB);
    }

} // namespace RayTracer

#endif
