#include "Audio.hpp"

//extern "C"
void audio_interrupt_handler()
{
    //api seems to require I check channels 1 at a time
    for (int i = 0; i < 12; i++)
    {
        if (dma_irqn_get_channel_status(_BaseAudio::IRQ_NUM, i))
        {
            auto aud = _BaseAudio::audio_callback_items[i];
            if (aud)
                aud->perform_interrupt();
            dma_irqn_acknowledge_channel(_BaseAudio::IRQ_NUM, i);
        }
    }
}

void _BaseAudio::enable_handler()
{
    static bool handler_enabled = false;
    if (!handler_enabled)
    {
        handler_enabled = true;
        irq_add_shared_handler(IRQ_NUM, audio_interrupt_handler, 200);
        irq_set_enabled(IRQ_NUM, true);
    }
}

std::array<_BaseAudio*, 12> _BaseAudio::audio_callback_items;
