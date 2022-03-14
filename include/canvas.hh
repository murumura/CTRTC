#ifndef CANVAS_HH
#define CANVAS_HH
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <primitives/vec.hh>
namespace RayTracer {

    template <std::size_t W, std::size_t H>
    class Canvas final {
    public:
        constexpr static std::size_t width = W;
        constexpr static std::size_t height = H;

        // canvas with black-color
        constexpr Canvas() noexcept : buffer{}
        {
        }

        constexpr std::size_t GetHeight() const noexcept
        {
            return height;
        }

        constexpr std::size_t GetWidth() const noexcept
        {
            return width;
        }

        constexpr auto& operator()(std::size_t row, std::size_t col)
        {
            return buffer[(row * width + col)];
        }

        constexpr auto operator()(std::size_t row, std::size_t col) const
        {
            return buffer[(row * width + col)];
        }

        constexpr void ToPPM(const std::string& fileName)
        {
            namespace fs = std::filesystem;
            std::ofstream ppm(fileName);
            if (!fs::path(fileName).has_extension() or fs::path(fileName).extension() != ".ppm")
                throw std::invalid_argument("Require 'ppm' as the file extention");
            ppm << "P3\n"
                << GetWidth() << " " << GetHeight() << "\n"
                << ColourConstants::MaxValue << "\n";

            int currentLineWidth = 0;

            for (auto i = 0; i < height; i++) {
                for (auto j = 0; j < width; j++) {
                    int r = static_cast<int>(
                        std::clamp(
                            ((*this)(i, j)[ColourConstants::r]) * ColourConstants::MaxValue, 0.0, (double)ColourConstants::MaxValue));
                    int g = static_cast<int>(
                        std::clamp(
                            ((*this)(i, j)[ColourConstants::g]) * ColourConstants::MaxValue, 0.0, (double)ColourConstants::MaxValue));
                    int b = static_cast<int>(
                        std::clamp(
                            ((*this)(i, j)[ColourConstants::b]) * ColourConstants::MaxValue, 0.0, (double)ColourConstants::MaxValue));

                    auto valueWidth = MathUtils::NumOfDigits(r, g, b);

                    if (currentLineWidth + 1 + valueWidth > 70) {
                        ppm << "\n";
                        currentLineWidth = 0;
                    }
                    ppm << r << " " << g << " " << b << "\n";
                    currentLineWidth += valueWidth;
                }
            }

            ppm.close();
        }

    private:
        std::array<Colour, width * height> buffer;
    };

} // namespace RayTracer

#endif
