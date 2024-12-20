#include <Audio.h>
#include <avr/io.h>
#include <util/delay.h>

Audio::Audio()
    : timer1_overflow_count{0}
    , current_sound{0}
    , current_note{0}
    , remaining_note_time{0}
    , is_playing_sound{false}
    , firstNoteStartTimeIsSet{false}

    // note, duration [in 32ms ticks]
    , buttonPress{
        NoteDuration{C4, NOTELENGTH_SHORT}
        } 
    , startUp{
        NoteDuration{C4, NOTELENGTH_SHORT},
        NoteDuration{E4, NOTELENGTH_SHORT},
        NoteDuration{G4, NOTELENGTH_SHORT},
        NoteDuration{C5, NOTELENGTH_SHORT}
    }
    , gameOver{
        NoteDuration{G4, NOTELENGTH_MEDIUM},
        NoteDuration{FS4, NOTELENGTH_MEDIUM},
        NoteDuration{F4, NOTELENGTH_MEDIUM},
        NoteDuration{E4, NOTELENGTH_MEDIUM},
        NoteDuration{DS4, NOTELENGTH_MEDIUM}
    }
    , gameWin{
        NoteDuration{C4, NOTELENGTH_MEDIUM},
        NoteDuration{E4, NOTELENGTH_MEDIUM},
        NoteDuration{G4, NOTELENGTH_MEDIUM},
        NoteDuration{C5, NOTELENGTH_LONG}
    }
    , moleUp{
        NoteDuration{C4, NOTELENGTH_SHORT},
        NoteDuration{G4, NOTELENGTH_SHORT}
    }
    , moleDown{
        NoteDuration{G4, NOTELENGTH_SHORT},
        NoteDuration{C4, NOTELENGTH_SHORT}
    }
    , hammerHit{
        NoteDuration{G4, NOTELENGTH_SHORT},
        NoteDuration{C5, NOTELENGTH_SHORT}
    }
    , hammerMiss{
        NoteDuration{C5, NOTELENGTH_SHORT},
        NoteDuration{FS4, NOTELENGTH_SHORT}
    }
    , themeSong0{ // 3 measures of bass
        NoteDuration{C4, NOTELENGTH_MEDIUM},
        NoteDuration{REST, NOTELENGTH_MEDIUM},
        NoteDuration{G3, NOTELENGTH_MEDIUM},
        NoteDuration{REST, NOTELENGTH_MEDIUM},
        NoteDuration{D4, NOTELENGTH_MEDIUM},
        NoteDuration{REST, NOTELENGTH_MEDIUM},
        NoteDuration{G3, NOTELENGTH_MEDIUM},
        NoteDuration{REST, NOTELENGTH_MEDIUM},

        NoteDuration{G4, NOTELENGTH_MEDIUM},
        NoteDuration{REST, NOTELENGTH_MEDIUM},
        NoteDuration{G3, NOTELENGTH_MEDIUM},
        NoteDuration{REST, NOTELENGTH_MEDIUM},
        NoteDuration{C4, NOTELENGTH_MEDIUM},
        NoteDuration{REST, NOTELENGTH_MEDIUM},
        NoteDuration{G3, NOTELENGTH_MEDIUM},
        NoteDuration{REST, NOTELENGTH_MEDIUM},

        NoteDuration{C4, NOTELENGTH_MEDIUM},
        NoteDuration{REST, NOTELENGTH_MEDIUM},
        NoteDuration{G3, NOTELENGTH_MEDIUM},
        NoteDuration{REST, NOTELENGTH_MEDIUM},
        NoteDuration{C4, NOTELENGTH_MEDIUM},
        NoteDuration{REST, NOTELENGTH_MEDIUM},
        NoteDuration{G3, NOTELENGTH_MEDIUM},
        NoteDuration{REST, NOTELENGTH_MEDIUM}
    }
    , themeSong1{
        NoteDuration{C5, NOTELENGTH_MEDIUM},
        NoteDuration{REST, NOTELENGTH_MEDIUM},
        NoteDuration{C5, NOTELENGTH_MEDIUM},
        NoteDuration{REST, NOTELENGTH_MEDIUM},
        NoteDuration{D5, NOTELENGTH_MEDIUM},
        NoteDuration{REST, NOTELENGTH_MEDIUM},
        NoteDuration{D5, NOTELENGTH_MEDIUM},
        NoteDuration{REST, NOTELENGTH_MEDIUM},

        NoteDuration{G5, NOTELENGTH_DOTTED_MEDIUM},
        NoteDuration{E5, NOTELENGTH_MEDIUM},
        NoteDuration{C5, NOTELENGTH_MEDIUM},
        NoteDuration{REST, 24}, // rest for 3 mediums

        NoteDuration{REST, 64} // rest for 8 mediums / 1 bar

    }
{
}

void setPrescaler1() {
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS20);
}

void setPrescaler8() {
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS21);
}

void setPrescaler32() {
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS21) | (1 << CS20);
}

void setPrescaler64() {
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS22);
}

void setPrescaler128() {
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS22) | (1 << CS20);
}

void setPrescaler256() {
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS22) | (1 << CS21);
}

void setPrescaler1024() {
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
}

// Function that returns the OCR value for a given frequency and sets the prescaler accordingly
uint8_t Audio::freqToOCRTop(uint16_t freq) {
    // Available prescaler values for Timer2
    const uint16_t prescalerValues[] = {1, 8, 32, 64, 128, 256, 1024};

    uint8_t bestOcrTop = 0;
    uint16_t bestPrescaler = 0;

    // Iterate over the available prescaler values to find the best prescaler and OCR value
    for (uint8_t i = 0; i < 6; i++) {
        uint16_t prescaler = prescalerValues[i];
        uint32_t ocrTop = (F_CPU / (2UL * prescaler * freq)) - 1;

        // Check if ocrTop fits in 8 bits
        if (ocrTop <= 255) {
            // Prefer higher ocrTop value for better resolution
            if (ocrTop > bestOcrTop) {
                bestOcrTop = ocrTop;
                bestPrescaler = prescaler;
            }
        }
    }

    // If a suitable prescaler was found, set it
    if (bestPrescaler != 0) {
        switch (bestPrescaler) {
            case 1:
                setPrescaler1();
                break;
            case 8:
                setPrescaler8();
                break;
            case 32:
                setPrescaler32();
                break;
            case 64:
                setPrescaler64();
                break;
            case 128:
                setPrescaler128();
                break;
            case 256:
                setPrescaler256();
                break;
            case 1024:
                setPrescaler1024();
                break;
        }
    } else {
        // If no suitable prescaler was found, use the lowest frequency possible
        setPrescaler1024();
        bestOcrTop = 255;
    }

    return bestOcrTop;
}

void Audio::setTimingVariable(uint32_t* timer1_overflow_count) {
    if (timer1_overflow_count == nullptr) {
        // Handle the error, e.g., set a flag or assert
        return;
    }
    this->timer1_overflow_count = timer1_overflow_count;
}

void Audio::init(){
    // Set pin 3 as output for the speaker
    DDRD |= (1 << PD3);

    // Set up Timer2 for PWM
    TCCR2A = (1 << WGM21) | (1 << WGM20) | (1 << COM2B1);  // Fast PWM, clear OC2B on compare match
    TCCR2B = (1 << WGM22) | (1 << CS22) | (1 << CS20);     // Fast PWM with prescaler of 64
}

void Audio::handleTimer1ISR() {
    if (!is_playing_sound) {
        disablePWM();
        return;
    }

    switch (current_sound) {
        case ButtonPress:
            audioPlayer(buttonPress, buttonPress_LENGTH);
            break;
        case StartUp:
            audioPlayer(startUp, startUp_LENGTH);
            break;
        case GameOver:
            audioPlayer(gameOver, gameOver_LENGTH);
            break;
        case GameWin:
            audioPlayer(gameWin, gameWin_LENGTH);
            break;
        case MoleUp:
            audioPlayer(moleUp, moleUp_LENGTH);
            break;
        case MoleDown:
            audioPlayer(moleDown, moleDown_LENGTH);
            break;
        case HammerHit:
            audioPlayer(hammerHit, hammerHit_LENGTH);
            break;
        case HammerMiss:
            audioPlayer(hammerMiss, hammerMiss_LENGTH);
            break;
        case ThemeSong0:
            audioPlayer(themeSong0, themeSong0_LENGTH);
            break;
        case ThemeSong1:
            audioPlayer(themeSong1, themeSong1_LENGTH);
            break;
    }
}

void Audio::enablePWM() {
    TCCR2A |= (1 << COM2B0) | (1 << COM2B1);
}

void Audio::disablePWM() {
    TCCR2A &= ~((1 << COM2B0) | (1 << COM2B1));
    PORTD &= ~(1 << PD3); // Turn off the speaker for good measure
}

void Audio::audioPlayer(NoteDuration *sound_array, uint8_t sound_array_length) {
    if (!firstNoteStartTimeIsSet) {
        note_start_time = *timer1_overflow_count;
        firstNoteStartTimeIsSet = true;
    }

    // Check if the current note is complete
    if (*timer1_overflow_count - note_start_time >= sound_array[current_note].duration) {
        current_note++;

        if (current_note >= sound_array_length) {
            is_playing_sound = false;  // End of sound is reached, stop playback
            disablePWM();
            return;
        }
        note_start_time = *timer1_overflow_count;  // Start the next note
    }

    // Handle rest or note
    if (sound_array[current_note].note == 0) {
        // Rest: disable PWM
        disablePWM();
    } else {
        // Note: enable PWM and set frequency
        enablePWM();
        uint8_t ocrTop = freqToOCRTop(sound_array[current_note].note);
        OCR2A = ocrTop;
    }
}

void Audio::stopSound() {
    is_playing_sound = false;
    disablePWM();
}

void Audio::playSound(Sound sound) {
    if (is_playing_sound || timer1_overflow_count == nullptr) {
        return;
    }

    // reset associated variables
    current_note = 0;
    remaining_note_time = 0;
    note_start_time = 0;
    is_playing_sound = true;
    firstNoteStartTimeIsSet = false;

    current_sound = sound;
}

void Audio::test_one_by_one() {
    while (1) {
        _delay_ms(1000);
        playSound(ButtonPress);
        _delay_ms(1000);
        playSound(StartUp);
        _delay_ms(1000);
        playSound(GameOver);
        _delay_ms(1000);
        playSound(GameWin);
        _delay_ms(1000);
        playSound(MoleUp);
        _delay_ms(1000);
        playSound(MoleDown);
        _delay_ms(1000);
        playSound(HammerHit);
        _delay_ms(1000);
        playSound(HammerMiss);
        _delay_ms(1000);
        playSound(ThemeSong0);
        _delay_ms(1000);
        playSound(ThemeSong1);
    }
}