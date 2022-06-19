#ifndef SHADDING_HH
#define SHADDING_HH
#include <variant>
#include <vec.hh>
namespace RayTracer {

    class PointLight {
    public:
        Tuple position;
        Colour intensity;
        constexpr PointLight(const Tuple& position_, const Colour& intensity_) noexcept
            : position{position_}, intensity{intensity_} {}
    };

    class Material {
    public:
        Colour color{PredefinedColours::WHITE};
        double ambient{0.1};
        double diffuse{0.9};
        double specular{0.9};
        double shininess{200.0};

        constexpr friend bool operator==(const Material& lhs,
            const Material& rhs) noexcept
        {
            // clang-format off
            return lhs.color == rhs.color && 
                lhs.ambient == rhs.ambient && 
                lhs.diffuse == rhs.diffuse && 
                lhs.specular == rhs.specular && 
                lhs.shininess == rhs.shininess;
            // clang-format on
        }
    };

    [[nodiscard]] constexpr Colour lighting(
        const Material& material,
        const PointLight& light,
        const Tuple& point,
        const Tuple& eye,
        const Tuple& normal) noexcept
    {
        // combine the surface color with the light's color/intensity
        const Colour effectiveColour = material.color * light.intensity;

        // find the direction to the light source
        const Tuple lightDirection = ToNormalizedVector(light.position - point);

        // compute the ambient contribution
        const Colour ambient = effectiveColour * material.ambient;

        // light_dot_normal represent the cosine of the angle between the
        // light vector and the normal vector.
        // A negative number means the light is on the other side of the surface
        const double lightDotNormal = lightDirection.DotProduct(normal);

        // precompute for ambient contribution
        const auto reflectv = -lightDirection.Reflect(normal);
        const double reflectDotEye = reflectv.DotProduct(eye);

        // compute the diffuse contribution
        const auto diffuse = (lightDotNormal < 0) ? PredefinedColours::BLACK
                                                  : effectiveColour * material.diffuse * lightDotNormal;
        // clang-format off
        #ifdef COMPILETIME 
        const auto specular = (lightDotNormal < 0 || reflectDotEye <= 0) ? PredefinedColours::BLACK
                                                : light.intensity * material.specular * MathUtils::ConstExprExp(reflectDotEye, material.shininess);
        # else
        const auto specular = (lightDotNormal < 0 || reflectDotEye <= 0) ? PredefinedColours::BLACK
                                                : light.intensity * material.specular * std::pow(reflectDotEye, material.shininess);
        #endif
        // clang-format on
        return ambient + diffuse + specular;
    }

} // namespace RayTracer

#endif
