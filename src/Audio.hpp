#ifndef H_DB5EAFBD40EE4ACDADB973F02E1A4AF1
#define H_DB5EAFBD40EE4ACDADB973F02E1A4AF1

#include <array>
#include <cstdint>
#include <hardware/dma.h>
#include <hardware/irq.h>

//extern "C"
void audio_interrupt_handler();

//allows audio classes of different sizes to share interrupt
class _BaseAudio
{
    public:
        using interrupt_t = void(*)(uint8_t* write_to, uint8_t* dont_write_to, size_t size);
        static constexpr float sys_tick_freq = 133'000'000;
        static constexpr unsigned int IRQ_NUM = DMA_IRQ_1;
        virtual void perform_interrupt() = 0;
        static std::array<_BaseAudio*, 12> audio_callback_items;
    
    protected:
        friend void audio_interrupt_handler();
        static void enable_handler();
};

template <std::size_t buffer_size>
class Audio : public _BaseAudio
{
    public:
        //claims and configures dma resources
        bool init(void* write_address)
        {
            deinit();
            dma_channel_a = dma_claim_unused_channel(false);
            if (dma_channel_a < 0)
                return false;
            audio_callback_items[dma_channel_a] = this;
            dma_channel_b = dma_claim_unused_channel(false);
            if (dma_channel_b < 0)
            {
                dma_channel_unclaim(dma_channel_a);
                audio_callback_items[dma_channel_a] = nullptr;
                dma_channel_a = -1;
                return false;
            }
            audio_callback_items[dma_channel_b] = this;

            dma_channel_config dconf;
            channel_config_set_read_increment(&dconf, true);
            channel_config_set_write_increment(&dconf, false);
            timer = dma_claim_unused_timer(false);
            if (timer < 0)
            {
                dma_channel_unclaim(dma_channel_a);
                dma_channel_unclaim(dma_channel_b);
                audio_callback_items[dma_channel_a] = audio_callback_items[dma_channel_b] = nullptr;
                dma_channel_a = dma_channel_b = -1;
                return false;
            }
            channel_config_set_dreq(&dconf, dma_get_timer_dreq(timer));
            channel_config_set_transfer_data_size(&dconf, DMA_SIZE_8);

            //create a second config so the triggers can point to each other
            dma_channel_config dconf_b = dconf;
            channel_config_set_chain_to(&dconf, dma_channel_b);
            channel_config_set_chain_to(&dconf, dma_channel_a);

            dma_channel_configure(dma_channel_a, &dconf, write_address, bankA.data(), buffer_size, false);
            dma_channel_configure(dma_channel_b, &dconf_b, write_address, bankB.data(), buffer_size, false);

            enable_handler();
            dma_irqn_set_channel_enabled(IRQ_NUM, dma_channel_a, true);
            dma_irqn_set_channel_enabled(IRQ_NUM, dma_channel_b, true);

            return true;
        }
        void deinit()
        {
            if (dma_channel_a >= 0)
            {
                dma_channel_cleanup(dma_channel_a);
                dma_channel_unclaim(dma_channel_a);
                dma_irqn_set_channel_enabled(IRQ_NUM, dma_channel_a, false);
                audio_callback_items[dma_channel_a] = nullptr;
                dma_channel_a = -1;
            }
            if (dma_channel_b >= 0)
            {
                dma_channel_cleanup(dma_channel_b);
                dma_channel_unclaim(dma_channel_b);
                dma_irqn_set_channel_enabled(IRQ_NUM, dma_channel_a, false);
                audio_callback_items[dma_channel_b] = nullptr;
                dma_channel_b = -1;
            }
            if (timer >= 0)
            {
                dma_timer_unclaim(timer);
                timer = -1;
            }
        }
        bool is_initialized() const
        {
            return dma_channel_a >= 0 && dma_channel_b >= 0;
        }
        std::array<uint8_t, buffer_size>& get_inactive_buffer()
        {
            return bankA_active ? bankB : bankA;
        }
        std::array<uint8_t, buffer_size>& get_active_buffer()
        {
            return bankA_active ? bankA : bankB;
        }
        //this callback is responsible for loading data into the unused buffer
        //(or triggering it to happen outside of interrupt context)
        void set_callback(interrupt_t new_callback)
        {
            callback = new_callback;
        }
        //need an algorithm for this
        // void set_sample_rate(float sample_rate)
        // {
        //     float ratio = sys_tick_freq / sample_rate;

        // }
        void set_sample_fraction(uint16_t num, uint16_t den)
        {
            if (timer >= 0)
                dma_timer_set_fraction(timer, num, den);
        }
        void start()
        {
            dma_channel_start(dma_channel_a);
        }

    protected:
        int get_active_channel() const
        {
            return bankA_active ? dma_channel_a : dma_channel_b;
        }
        int get_inactive_channel() const
        {
            return bankA_active ? dma_channel_b : dma_channel_a;
        }
        void perform_interrupt() override
        {
            bankA_active = !bankA_active;
            dma_channel_set_read_addr(get_inactive_channel(), get_inactive_buffer().data(), false);
            auto& just_finished = get_inactive_buffer();
            auto& now_active = get_active_buffer();
            if (callback)
                callback(just_finished.data(), now_active.data(), buffer_size);
        }

    private:
        std::array<uint8_t, buffer_size> bankA;
        std::array<uint8_t, buffer_size> bankB;
        int dma_channel_a = -1;
        int dma_channel_b = -1;
        int timer = -1;
        bool bankA_active = true;
        interrupt_t callback;
};

#endif //H_DB5EAFBD40EE4ACDADB973F02E1A4AF1
