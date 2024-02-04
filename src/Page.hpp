#ifndef H_1AAB801F01BA49D0BB2AAB917C307E50
#define H_1AAB801F01BA49D0BB2AAB917C307E50

#include <array>
#include "Button.hpp"
#include "ConfigError.hpp"

namespace Configuration
{
    class Page
    {
        static std::array<Pixel, 12> led_rgb;
        std::array<Button, 12> buttons;
        std::string display_text;
        public:
            inline Page() { }
            inline Page(const Page& other) 
            {
                buttons = other.buttons;
                display_text = other.display_text;
            }
            inline Page(Page&& other) : buttons(std::move(other.buttons)) { }
            ConfigError load();
            void update_leds();
            inline Button& button(std::size_t index)
            {
                return buttons[index];
            }
    };
}

#endif //H_1AAB801F01BA49D0BB2AAB917C307E50
