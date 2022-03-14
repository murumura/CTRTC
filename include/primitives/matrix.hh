#ifndef MAT_HH
#define MAT_HH
#include <array>
#include <primitive_traits.hh>
#include <primitives/vec.hh>
#include <type_traits>
#include <utils/math.hh>

namespace RayTracer {

    template <typename T, std::size_t R, std::size_t C, typename = typename std::enable_if_t<std::is_arithmetic_v<T>, T>>
    class Matrix {
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

        friend std::ostream& operator<<(std::ostream& stream, const Matrix& mat) noexcept
        {
            stream << "Matrix:( \n";
            for (std::size_t i = 0; i < mat.Rows; ++i)
                for (std::size_t j = 0; j < mat.Cols; ++j)
                    stream << mat[i][j] << (j == mat.Cols - 1 ? "\n" : ", ");

            return stream << ")";
        }

        T contents[Rows][Cols];
    };

    template <typename T, std::size_t R, std::size_t C>
    Matrix(const T (&)[R][C]) -> Matrix<T, R, C>;

    namespace MatrixUtils {
        template <std::size_t R, std::size_t C, typename Func>
        constexpr decltype(auto) Generate(Func&& func) noexcept
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
        constexpr Matrix<T, R, C> ElementWise(Func func, const Matrix<T, R, C>& mat, const Matrices&... matrices) noexcept
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
    constexpr bool operator==(const Matrix<T, R, C>& matA, const Matrix<T, R, C>& matB) noexcept
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
    constexpr bool operator!=(const Matrix<T, R, C>& matA, const Matrix<T, R, C>& matB) noexcept
    {
        return !(matA == matB);
    }

    /** Numerical negative, element-wise. */
    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator-(const Matrix<T, R, C>& mat) noexcept
    {
        return MatrixUtils::ElementWise([](T x) { return -1.0 * x; }, mat);
    }

    /* Element-wise add */
    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator+(const Matrix<T, R, C>& mat, T scalar) noexcept
    {
        return MatrixUtils::ElementWise([scalar](T x) { return x + scalar; }, mat);
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator+(T scalar, const Matrix<T, R, C>& mat) noexcept
    {
        return MatrixUtils::ElementWise([scalar](T x) { return x + scalar; }, mat);
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator+(const Matrix<T, R, C>& matA, const Matrix<T, R, C>& matB) noexcept
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
    constexpr Matrix<T, R, C> operator-(const Matrix<T, R, C>& mat, T scalar) noexcept
    {
        return MatrixUtils::ElementWise([scalar](T x) { return x - scalar; }, mat);
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator-(const Matrix<T, R, C>& matA, const Matrix<T, R, C>& matB) noexcept
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
    constexpr Matrix<T, R, C> operator*(const Matrix<T, R, C>& mat, T scalar) noexcept
    {
        return MatrixUtils::ElementWise([scalar](T x) { return scalar * x; }, mat);
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator*(T scalar, const Matrix<T, R, C>& mat) noexcept
    {
        return MatrixUtils::ElementWise([scalar](T x) { return scalar * x; }, mat);
    }

    template <typename T, std::size_t R1, std::size_t C, std::size_t C2>
    constexpr Matrix<T, R1, C2> operator*(const Matrix<T, R1, C>& matA, const Matrix<T, C, C2>& matB) noexcept
    {
        Matrix<T, R1, C2> ret{};
        for (std::size_t row = 0u; row < R1; ++row)
            for (std::size_t col = 0u; col < C2; ++col)
                for (std::size_t c = 0u; c < C; ++c)
                    ret[row][col] += matA[row][c] * matB[c][col];
        return ret;
    }

    // mat * mat only support square matrices
    template <typename T, std::size_t D>
    constexpr Matrix<T, D, D>& operator*=(Matrix<T, D, D>& matA, const Matrix<T, D, D>& matB) noexcept
    {
        // since we need to write to A
        Matrix<T, D, D> temp = matA;
        matA = {};
        for (std::size_t row = 0u; row < D; ++row)
            for (std::size_t col = 0u; col < D; ++col)
                for (std::size_t c = 0u; c < D; ++c)
                    matA[row][col] += temp[row][c] * matB[c][col];
        return matA;
    }

    // mat * vec
    template <typename T, std::size_t R, std::size_t C>
    constexpr Vec<T, C> operator*(const Matrix<T, R, C>& mat, const Vec<T, C>& vec) noexcept
    {
        Vec<T, C> ret{};
        for (std::size_t row = 0; row < R; row++) {
            for (std::size_t col = 0; col < C; col++) {
                ret[row] += mat[row][col] * vec[col];
            }
        }
        return ret;
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C>& operator*=(Matrix<T, R, C>& mat, T scalar) noexcept
    {
        for (std::size_t row = 0u; row < R; row++) {
            for (std::size_t col = 0u; col < C; col++) {
                mat[row][col] *= scalar;
            }
        }
        return mat;
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C>& operator*=(T scalar, Matrix<T, R, C>& mat) noexcept
    {
        for (std::size_t row = 0u; row < R; row++) {
            for (std::size_t col = 0u; col < C; col++) {
                mat[row][col] *= scalar;
            }
        }
        return mat;
    }

    /* Element-wise division */
    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator/(const Matrix<T, R, C>& mat, T scalar) noexcept
    {
        return MatrixUtils::ElementWise([scalar](T x) { return x / scalar; }, mat);
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C>& operator/=(Matrix<T, R, C>& mat, T scalar) noexcept
    {
        for (std::size_t row = 0u; row < R; row++) {
            for (std::size_t col = 0u; col < C; col++) {
                mat[row][col] /= scalar;
            }
        }
        return mat;
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C> operator/(const Matrix<T, R, C>& matA, const Matrix<T, R, C>& matB) noexcept
    {
        return MatrixUtils::ElementWise(std::divides<T>(), matA, matB);
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R, C>& operator/=(Matrix<T, R, C>& matA, const Matrix<T, R, C>& matB) noexcept
    {
        for (std::size_t row = 0u; row < R; row++) {
            for (std::size_t col = 0u; col < C; col++) {
                matA[row][col] /= matB[row][col];
            }
        }
        return matA;
    }

    /************************
     * Matrix Utils         *
    *************************/

    /* forward decalation to avoid function dependency */
    template <typename T, std::size_t N>
    constexpr T Cofactor(const Matrix<T, N, N>&, std::size_t, std::size_t);

    template <typename T, std::size_t N>
    constexpr T Determinant(const Matrix<T, N, N>& mat);

    template <typename T, std::size_t N>
    constexpr Matrix<T, N, N> DiagonalMatrix(T diag)
    {
        return MatrixUtils::Generate<N, N>([diag](auto i, auto j) {
            if (i == j)
                return diag;
            return T{};
        });
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, C, R> Transpose(const Matrix<T, R, C>& mat)
    {
        return MatrixUtils::Generate<C, R>([&mat](auto i, auto j) {
            return mat[j][i];
        });
    }

    template <typename T, std::size_t R, std::size_t C>
    constexpr Matrix<T, R - 1, C - 1> SubMatrix(const Matrix<T, R, C>& mat, std::size_t omitRow, std::size_t omitCol)
    {
        Matrix<T, R - 1, C - 1> subMatrix{};
        for (std::size_t row = 0u, subRow = 0u; row < R; row++) {
            if (row == omitRow)
                continue;
            for (std::size_t col = 0u, subCol = 0u; col < C; col++) {
                if (col == omitCol)
                    continue;
                subMatrix[subRow][subCol] = mat[row][col];
                subCol++;
            }
            subRow++;
        }
        return subMatrix;
    }

    template <typename T>
    constexpr T Determinant(const Matrix<T, 2, 2>& mat)
    {
        return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
    }

    // minor(i,j) of matrix A is equal to det(sub(i,j)) of matrix A
    template <typename T, std::size_t N>
    constexpr T Minor(const Matrix<T, N, N>& mat, std::size_t omitRow, std::size_t omitCol)
    {
        return Determinant(SubMatrix(mat, omitRow, omitCol));
    }

    template <typename T, std::size_t N>
    constexpr T Cofactor(const Matrix<T, N, N>& mat, std::size_t omitRow, std::size_t omitCol)
    {
        return ((omitRow + omitCol) % 2 ? -1 : 1) * Minor(mat, omitRow, omitCol);
    }

    template <typename T, std::size_t N>
    constexpr T Determinant(const Matrix<T, N, N>& mat)
    {
        T result{};
        for (std::size_t col = 0u; col < N; col++)
            result += mat[0][col] * Cofactor(mat, 0, col);
        return result;
    }

    template <typename T, std::size_t N>
    constexpr bool Invertible(const Matrix<T, N, N>& mat)
    {
        return !MathUtils::ApproxEqual(Determinant(mat), 0);
    }

    template <typename T, std::size_t N>
    constexpr Matrix<T, N, N> Inverse(const Matrix<T, N, N>& mat)
    {
        assert(Invertible(mat));
        Matrix<T, N, N> result{};
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                result[col][row] = Cofactor(mat, row, col) / Determinant(mat);
            }
        }
        return result;
    }
    /************************
    * Matrix Constants      *
    *************************/

    struct PredefinedMatrices {
        template <typename T = double, std::size_t N = 4>
        static constexpr Matrix<T, N, N> I = DiagonalMatrix<T, N>(1.0);
    };

} // namespace RayTracer
#endif
