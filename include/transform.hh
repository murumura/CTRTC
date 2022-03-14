#ifndef TRANSFORM_HH
#define TRANSFORM_HH
#include <primitives/matrix.hh>
#include <utils/math.hh>

namespace RayTracer {
    using TransformMatrix = Matrix<double, 4, 4>;
    /* Forward declaration */
    class Transform;

    namespace MatrixUtils {
        constexpr Transform Translation(double offsetX, double offsetY, double offsetZ);
        constexpr Transform Scale(double ratioX, double ratioY, double ratioZ);
        constexpr Transform RotateX(double radians);
        constexpr Transform RotateY(double radians);
        constexpr Transform RotateZ(double radians);
        constexpr Transform Shearing(double xy, double xz, double yx, double yz, double zx, double zy);
    } // namespace MatrixUtils

    class Transform : public TransformMatrix {
    public:
        constexpr Transform() noexcept : TransformMatrix{PredefinedMatrices::I<double, 4>} {};

        constexpr Transform(const TransformMatrix& other) noexcept : TransformMatrix(other) {}

        constexpr Transform(TransformMatrix&& other) noexcept : TransformMatrix(std::move(other)) {}

        constexpr Transform(const Transform& other) noexcept : TransformMatrix(other) {}

        constexpr Transform(Transform&& other) noexcept : TransformMatrix(std::move(other)) {}

        constexpr Transform Translation(double offsetX, double offsetY, double offsetZ)
        {
            return MatrixUtils::Translation(offsetX, offsetY, offsetZ) * (*this);
        }

        constexpr Transform Scale(double ratioX, double ratioY, double ratioZ)
        {
            return MatrixUtils::Scale(ratioX, ratioY, ratioZ) * (*this);
        }

        constexpr Transform RotateX(double radians)
        {
            return MatrixUtils::RotateX(radians) * (*this);
        }

        constexpr Transform RotateY(double radians)
        {
            return MatrixUtils::RotateY(radians) * (*this);
        }

        constexpr Transform RotateZ(double radians)
        {
            return MatrixUtils::RotateZ(radians) * (*this);
        }

        constexpr Transform Shearing(double xy, double xz, double yx, double yz, double zx, double zy)
        {
            return MatrixUtils::Shearing(xy, xz, yx, yz, zx, zy) * (*this);
        }

        /**
         * NOTE: Apply chaining in reverse order
        */
        template <typename... Args, typename = std::enable_if_t<(std::is_same_v<Transform, std::decay_t<Args>> && ...)>>
        constexpr Transform Chain(Args&&... args)
        {
            return (... * args) * (*this);
        }
    };

    namespace MatrixUtils {
        constexpr Transform Translation(double offsetX, double offsetY, double offsetZ)
        {
            TransformMatrix translation = PredefinedMatrices::I<double, 4>;
            translation[0][3] = offsetX;
            translation[1][3] = offsetY;
            translation[2][3] = offsetZ;
            return translation;
        }

        constexpr Transform Scale(double ratioX, double ratioY, double ratioZ)
        {
            TransformMatrix scale = PredefinedMatrices::I<double, 4>;
            scale[0][0] = ratioX;
            scale[1][1] = ratioY;
            scale[2][2] = ratioZ;
            return scale;
        }

        constexpr Transform RotateX(double radians)
        {
            double sine = MathUtils::Sine(radians);
            double cosine = MathUtils::Cosine(radians);
            TransformMatrix rotateX = PredefinedMatrices::I<double, 4>;

            rotateX[1][1] = cosine;
            rotateX[1][2] = -sine;
            rotateX[2][1] = sine;
            rotateX[2][2] = cosine;
            return rotateX;
        }

        constexpr Transform RotateY(double radians)
        {
            double sine = MathUtils::Sine(radians);
            double cosine = MathUtils::Cosine(radians);
            TransformMatrix rotateY = PredefinedMatrices::I<double, 4>;
            rotateY[0][0] = cosine;
            rotateY[0][2] = sine;
            rotateY[2][0] = -sine;
            rotateY[2][2] = cosine;
            return rotateY;
        }

        constexpr Transform RotateZ(double radians)
        {
            double sine = MathUtils::Sine(radians);
            double cosine = MathUtils::Cosine(radians);
            TransformMatrix rotateZ = PredefinedMatrices::I<double, 4>;
            rotateZ[0][0] = cosine;
            rotateZ[0][1] = -sine;
            rotateZ[1][0] = sine;
            rotateZ[1][1] = cosine;
            return rotateZ;
        }

        constexpr Transform Shearing(double xy, double xz, double yx, double yz, double zx, double zy)
        {
            TransformMatrix shearing = PredefinedMatrices::I<double, 4>;
            shearing[0][1] = xy;
            shearing[0][2] = xz;
            shearing[1][0] = yx;
            shearing[1][2] = yz;
            shearing[2][0] = zx;
            shearing[2][1] = zy;
            return shearing;
        }

        constexpr Transform ViewTransform(const Tuple& from, const Tuple& to, const Tuple& up)
        {
            const auto forward = ToNormalizedVector(to - from);
            const auto left = forward.CrossProduct(ToNormalizedVector(up));
            const auto fixedUp = left.CrossProduct(forward);

            TransformMatrix orientation = PredefinedMatrices::I<double, 4>;
            orientation[0][0] = left[0];
            orientation[0][1] = left[1];
            orientation[0][2] = left[2];
            orientation[1][0] = fixedUp[0];
            orientation[1][1] = fixedUp[1];
            orientation[1][2] = fixedUp[2];
            orientation[2][0] = -forward[0];
            orientation[2][1] = -forward[1];
            orientation[2][2] = -forward[2];

            return orientation * Translation(-from[0], -from[1], -from[2]);
        }
    } // namespace MatrixUtils
} // namespace RayTracer
#endif
