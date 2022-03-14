#ifndef RAY_HH
#define RAY_HH
#include <primitives/vec.hh>
#include <transform.hh>
namespace RayTracer {
    class Ray final {
    private:
        Tuple origin, direction;

    public:
        /* Create a ray with origin located at (0,0,0) but no direction */
        constexpr explicit Ray() noexcept
            : origin{PredefinedTuples::ZeroPoint}, direction{PredefinedTuples::ZeroVector} {}

        constexpr explicit Ray(const Tuple& origin, const Tuple& direction)
            : origin{origin}, direction{direction}
        {
            if (!IsPoint(origin))
                throw std::invalid_argument("Ray requires origin to be a point-type");

            if (!IsVector(direction))
                throw std::invalid_argument("Ray requires direction to be a vector-type");
        }

        constexpr Ray(const Ray&) noexcept = default;
        constexpr Ray(Ray&&) noexcept = default;
        constexpr Ray& operator=(const Ray&) = default;
        constexpr Ray& operator=(Ray&&) noexcept = default;

        constexpr Tuple GetOrigin() const noexcept
        {
            return origin;
        }

        constexpr Tuple GetDirection() const noexcept
        {
            return direction;
        }

        constexpr Tuple PositionAlong(double t) const noexcept
        {
            return origin + t * direction;
        }

        /// Create a new Ray by applying a transformation to this one.
        constexpr Ray Transform(const Transform& transform) const noexcept
        {
            return Ray{transform * origin, transform * direction};
        }
    };
} // namespace RayTracer

#endif
