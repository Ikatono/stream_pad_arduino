#include "Page.hpp"

namespace Configuration
{
    std::array<Pixel, 12> Page::led_rgb;

    void Page::update_leds()
    {
        for (int i = 0; i < 12; i++)
            led_rgb[i] = buttons[i].color;
    }
}
