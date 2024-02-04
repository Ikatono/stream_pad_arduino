#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_NeoPixel.h>
#include <RotaryEncoder.h>
#include <hardware/pwm.h>
#include <hardware/gpio.h>
#include <hardware/regs/pwm.h>
#include "src/Config.hpp"
#include "src/Audio.hpp"
#include <limits>

#include "src/sine_8khz.hpp"
#include "src/200hz300hz_sr10khz_dur8sec.hpp"

constexpr float MIDDLE_C = 261.63;

inline int modulo(int a, int b) {
    if (b < 0)
        return modulo(-a, -b);
    const int result = a % b;
    return result >= 0 ? result : result + b;
}

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_NEOPIXEL, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &SPI1, OLED_DC, OLED_RST, OLED_CS);

StaticSemaphore_t encoder_semaphore_buffer;
SemaphoreHandle_t encoder_semaphore;

// Create the rotary encoder
RotaryEncoder encoder(PIN_ROTA, PIN_ROTB, RotaryEncoder::LatchMode::FOUR3);
void checkPosition()
{
    encoder.tick();
    xSemaphoreGiveFromISR(encoder_semaphore, nullptr);
} // just call tick() to check the state.
// our encoder position state
int encoder_pos = 0;

// Audio<2000> audioPlayer;

// struct audio_task_params
// {
//     size_t size;
//     const uint8_t* data;
// };

// audio_task_params params;
// TaskHandle_t audio_task_handle;

// void audio_interrupt_func(uint8_t* inactive, uint8_t* active, size_t buffer_size)
// {
//     vTaskNotifyGiveFromISR(audio_task_handle, nullptr);
// }

// void audio_task_func(void* task_parameters)
// {
//     auto& params = *reinterpret_cast<audio_task_params*>(task_parameters);
//     int offset = 0;
//     const uint8_t* end = params.data + params.size;
//     auto& first = audioPlayer.get_active_buffer();
//     auto& second = audioPlayer.get_inactive_buffer();
//     audioPlayer.set_callback(audio_interrupt_func);
//     memcpy(first.data(), params.data, first.size());
//     params.data += first.size();
//     memcpy(second.data(), params.data, second.size());
//     params.data += second.size();
//     pwm_set_enabled(pwm_gpio_to_slice_num(PIN_SPEAKER), true);
//     audioPlayer.start();
//     while(params.data + first.size() < end)
//     {
//         ulTaskNotifyTake(pdTRUE, std::numeric_limits<TickType_t>::max());
//         auto& inactive = audioPlayer.get_inactive_buffer();
//         memcpy(inactive.data(), params.data, inactive.size());
//         params.data += first.size();
//     }
//     while (1);
// }

void setup()
{
    encoder_semaphore = xSemaphoreCreateBinaryStatic(&encoder_semaphore_buffer);
    Serial.begin(115200);
    pixels.begin();
    pixels.setBrightness(255);
    pixels.show();
    // pixels.setPixelColor(3, 0xFFBF00);
    // pixels.show();
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);

    // set rotary encoder inputs and interrupts
    pinMode(PIN_ROTA, INPUT_PULLUP);
    pinMode(PIN_ROTB, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_ROTA), checkPosition, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ROTB), checkPosition, CHANGE);

    // Start OLED
    display.begin(0, true); // we dont use the i2c address but we will reset!
    //display.display();
    display.clearDisplay();
    // text display tests
    display.setTextSize(1);
    display.setTextWrap(false);
    display.setTextColor(SH110X_WHITE, SH110X_BLACK); // white text, black background

    // pinMode(PIN_SPEAKER_ENABLE, OUTPUT);
    // digitalWrite(PIN_SPEAKER_ENABLE, HIGH);
    // gpio_set_function(PIN_SPEAKER, GPIO_FUNC_PWM);
    // auto slice = pwm_gpio_to_slice_num(PIN_SPEAKER);
    // auto channel = pwm_gpio_to_channel(PIN_SPEAKER);
    // pwm_config conf;
    // pwm_config_set_output_polarity(&conf, false, false);
    // pwm_config_set_phase_correct(&conf, false);
    // pwm_config_set_clkdiv_mode(&conf, PWM_DIV_FREE_RUNNING);
    // pwm_config_set_wrap(&conf, 255);
    // pwm_init(slice, &conf, false);
    // pwm_set_chan_level(slice, channel, 128);
    // // if (!audioPlayer.init((void*)
    // //     (PWM_BASE + (slice * (PWM_CH1_CTR_OFFSET - PWM_CH0_CTR_OFFSET)
    // //     + PWM_CH0_CC_OFFSET + (channel ? 2 : 0)))))
    // if (!audioPlayer.init((void*)PWM_BASE + PWM_CH0_CC_OFFSET))
    //     digitalWrite(PIN_LED, HIGH);
    // audioPlayer.set_sample_fraction(1, 13300);
    // params.data = wave_200hz300hz_sr10khz_dur8sec;
    // params.size = sizeof(wave_200hz300hz_sr10khz_dur8sec);
    // auto result = xTaskCreate(audio_task_func, "Audio Task",
    //     1024, &params, 10, &audio_task_handle);

    // tone(PIN_SPEAKER, 400, 500);
    // pwm_set_enabled(pwm_gpio_to_slice_num(PIN_SPEAKER), true);

    // display.drawCircle(50, 50, 20, SH110X_WHITE);
    // display.display();
}

void loop()
{
    // display.printf("%d", random(9));
    // display.display();
    // delay(100);

    for (int i = 0; i < 12; i++)
        pixels.setPixelColor(i, (modulo(encoder.getPosition(), 12) == i) ? 0xFF0000 : 0);
    pixels.show();
    delay(50);
}
