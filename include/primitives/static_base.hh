#ifndef STATICBASE_HH
#define STATICBASE_HH
namespace RayTracer {
template <typename Derived, template <typename> class crtpType>
class StaticBase {
  StaticBase() = default;
  friend crtpType<Derived>;

 public:
  constexpr Derived& derived() { return static_cast<Derived&>(*this); }

  constexpr Derived const& derived() const {
    return static_cast<Derived const&>(*this);
  }
};
}  // namespace RayTracer
#endif
