#ifndef AUDIO_H
#define AUDIO_H

#include <avr/io.h>

class Audio {
    public:
        Audio();
        
        enum Sound : uint8_t {
            ThemeSong0 = 0,
            ThemeSong1 = 1,

            MoleUp = 2,
            MoleDown = 3,

            HammerHit = 4,
            HammerMiss = 5,

            GameOver = 6,
            GameWin = 7,

            ButtonPress = 8,
            StartUp = 9
        };

        enum Note : uint16_t {
            C4 = 262,
            CS4 = 277,
            D4 = 294,
            DS4 = 311,
            E4 = 330,
            F4 = 349,
            FS4 = 370,
            G4 = 392,
            GS4 = 415,
            A4 = 440,
            AS4 = 466,
            B4 = 494,
            C5 = 523,
            CS5 = 554,
            D5 = 587,
            DS5 = 622,
            E5 = 659,
            F5 = 698,
            FS5 = 740,
            G5 = 784,
            GS5 = 831,
            A5 = 880,
            AS5 = 932,
            B5 = 988,
            C6 = 1047,
            REST = 0
        };

        struct NoteDuration {
            Note note;
            uint8_t duration;
        };

        void init();
        void playSound(Sound sound);
        void handleTimer1ISR();
        void setTimingVariable(uint32_t* timer1_overflow_count);
    
    private:
        uint8_t freqToOCRTop(uint16_t freq);
        void audioPlayer(NoteDuration *sound_array, uint8_t sound_array_length);
        void enablePWM();
        void disablePWM();

        uint32_t* timer1_overflow_count;
        
        uint8_t current_sound;
        uint16_t current_note;
        uint16_t remaining_note_time;
        bool is_playing_sound;
        uint32_t note_start_time;
        bool firstNoteStartTimeIsSet;

        NoteDuration buttonPress[1];
        NoteDuration startUp[4];

};

#endif