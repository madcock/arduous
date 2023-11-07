#ifndef ARDUOUS_ARDUOUS_H
#define ARDUOUS_ARDUOUS_H

#include <array>
#include <bitset>
#include <cstdint>
#include <cstdio>
#include <vector>

#include "sim_avr.h"
extern "C" {
#include "ssd1306_virt.h"
}

constexpr unsigned int TIMING_FPS = 60;
#if !defined(SF2000)
constexpr unsigned int TIMING_SAMPLE_RATE = 48000;
#else
constexpr unsigned int TIMING_SAMPLE_RATE = 11025;
#endif
constexpr unsigned int DISPLAY_WIDTH = 128;
constexpr unsigned int DISPLAY_HEIGHT = 64;

struct ArduousButtonState {
    bool buttonUp = false;
    bool buttonRight = false;
    bool buttonLeft = false;
    bool buttonDown = false;
    bool buttonA = false;
    bool buttonB = false;
};

class Arduous {
    struct PinCallbackParamT {
        Arduous* self;
        int speakerPin;
    };

   public:
    Arduous();
    Arduous(const Arduous&) = delete;
    Arduous(Arduous&&) = delete;
    Arduous& operator=(const Arduous&) = delete;
    Arduous& operator=(Arduous&&) = delete;
    ~Arduous() = default;

    void loadHexBuffer(const char *data, size_t sz);
    void init(uint8_t* boot, uint32_t bootSize, uint32_t bootBase);
    void reset();
    void emulateFrame();
    void update(int steps = 1);
    void setButtonState(ArduousButtonState newButtonState);

    std::bitset<DISPLAY_WIDTH * DISPLAY_HEIGHT> getVideoFrameBuffer();
    std::pair<uint8_t, uint8_t> getVideoColors();
    std::vector<int16_t> getAudioBuffer();
    size_t getSaveSize();
    bool save(void* data, size_t size);
    bool load(const void* data, size_t size);

    size_t getRamSize();
    void *getRam();
    size_t getEEPROMSize();
    void *getEEPROM();
    void *getVideoRAM();
    size_t getVideoRAMSize();

   private:
    // Atcore cpu;
    avr_t* cpu;
    // SSD1306 screen;
    ssd1306_t screen;

    std::string mmcu = "atmega32u4";
    uint64_t freq = 16000000;
    uint64_t cyclesPerVideoFrame;
    uint64_t cyclesPerAudioSample;
    unsigned int audioSamplesPerVideoFrame;
    uint64_t frameStartCycle;
    uint64_t frameEndCycle;

    ArduousButtonState buttonState = {};
    std::array<PinCallbackParamT, 2> pinCallbackParamTs;

    std::bitset<2> speakerPins;
    std::vector<int16_t> audioBuffer;

    int16_t getCurrentSpeakerSample();
    void extendAudioBuffer();

    static void soundPinCallback(struct avr_irq_t* irq, uint32_t value, void* param);
};

#endif
