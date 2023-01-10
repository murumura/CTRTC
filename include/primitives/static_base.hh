#ifndef STATICBASE_HH
#define STATICBASE_HH
namespace RayTracer {
template <typename Derived, template <typename> class CrtpType>
class StaticBase {
  StaticBase() = default;
  friend CrtpType<Derived>;

 public:
  constexpr Derived& derived() { return static_cast<Derived&>(*this); }

  constexpr Derived const& derived() const {
    return static_cast<Derived const&>(*this);
  }
};
}  // namespace RayTracer
#endif
