#ifndef AUDIO_H
#define AUDIO_H

#include <avr/io.h>
#include <Timer1Overflow.h>

// volume control
#define PWM_DUTY_CYCLE_DIVIDER 20 // divider in the context of 100/x. So a value of 2 will result in a 50% duty cycle

#define NOTELENGTH_SHORT 4
#define NOTELENGTH_MEDIUM 8
#define NOTELENGTH_DOTTED_MEDIUM 12
#define NOTELENGTH_LONG 16

#define startUp_LENGTH 4
#define gameOver_LENGTH 5
#define gameWin_LENGTH 4
#define moleUp_LENGTH 2
#define hammerHit_LENGTH 2
#define hammerMiss_LENGTH 2
#define themeSong0_LENGTH 24

class Audio
{
public:
    Audio(Timer1Overflow &timer1);

    enum Sound : uint8_t
    {
        ThemeSong0 = 0,

        MoleUp = 2,

        HammerHit = 4,
        HammerMiss = 5,

        GameOver = 6,
        GameWin = 7,

        StartUp = 9
    };

    // only need notes:
    // C3, DS3, E3, F3, FS3, G3, C4
    // note frequencies are in Hz
    enum Note : uint16_t
    {
        G2 = 98,
        C3 = 131,
        D3 = 147,
        G3 = 196,

        FS3 = 185,
        F3 = 174,
        E3 = 165,
        DS3 = 155,

        C4 = 262,
        D4 = 294,
        DS4 = 311,
        E4 = 330,
        F4 = 349,
        FS4 = 370,
        G4 = 392,

        C5 = 523,
        D5 = 587,
        E5 = 659,
        G5 = 784,

        REST = 0
    };

    struct NoteDuration
    {
        Note note;
        uint8_t duration;
    };

    void init();
    void playSound(Sound sound);
    void stopSound();
    void handleTimer1ISR();
    void test_one_by_one();

private:
    uint8_t freqToOCRTop(uint16_t freq);
    void audioPlayer(NoteDuration *sound_array, uint8_t sound_array_length);
    void enablePWM();
    void disablePWM();

    uint8_t current_sound;
    uint16_t current_note;
    bool is_playing_sound;
    uint32_t note_start_time;
    bool firstNoteStartTimeIsSet;

    Timer1Overflow *timer1;

    NoteDuration startUp[startUp_LENGTH];
    NoteDuration gameOver[gameOver_LENGTH];
    NoteDuration gameWin[gameWin_LENGTH];
    NoteDuration moleUp[moleUp_LENGTH];
    NoteDuration hammerHit[hammerHit_LENGTH];
    NoteDuration hammerMiss[hammerMiss_LENGTH];
    NoteDuration themeSong0[themeSong0_LENGTH];
};

#endif