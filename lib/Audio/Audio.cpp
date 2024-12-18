#include <Audio.h>
#include <avr/io.h>
#include <HardwareSerial.h>

Audio::Audio()
    : timer1_overflow_count{0}
    , current_sound{0}
    , current_note{0}
    , remaining_note_time{0}
    , is_playing_sound{false}
    , firstNoteStartTimeIsSet{false}

    // note, duration [in 32ms ticks]
    , buttonPress{
        NoteDuration{C4, 4}
        } 
    , startUp{
        NoteDuration{C3, 4},
        NoteDuration{E3, 4},
        NoteDuration{G3, 4},
        NoteDuration{C3, 4}
    }
    , gameOver{
        NoteDuration{G3, 8},
        NoteDuration{FS3, 8},
        NoteDuration{F3, 8},
        NoteDuration{E3, 8},
        NoteDuration{D3, 8}
    }
    , gameWin{
        NoteDuration{C3, 8},
        NoteDuration{E3, 8},
        NoteDuration{G3, 8},
        NoteDuration{C4, 16}
    }
    , moleUp{
        NoteDuration{C3, 4},
        NoteDuration{G3, 4}
    }
    , moleDown{
        NoteDuration{G3, 4},
        NoteDuration{C3, 4}
    }
    , hammerHit{
        NoteDuration{G3, 4},
        NoteDuration{C4, 4}
    }
    , hammerMiss{
        NoteDuration{C4, 4},
        NoteDuration{FS3, 4}
    }
{
}

void setPrescaler32() {
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS21) | (1 << CS20);
}

void setPrescaler64() {
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS22);
}

void setPrescaler256() {
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS22) | (1 << CS21);
}

// function that returns the OCR value for a given frequency, and sets the prescaler accordingly
uint8_t Audio::freqToOCRTop(uint16_t freq) {
    uint16_t ocrTop = (F_CPU / (2 * 64 * freq)) - 1;
    if (ocrTop > 255) {
        // setPrescaler256();
        // ocrTop = (F_CPU / (2 * 256 * freq)) - 1;
        setPrescaler32();
        ocrTop = (F_CPU / (2 * 32 * freq)) - 1;
    } else if (ocrTop < 64) {
        setPrescaler256();
        ocrTop = (F_CPU / (2 * 256 * freq)) - 1;
    } else {
        setPrescaler64();
    }
    return ocrTop;
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
            audioPlayer(buttonPress, 1);
            break;
        case StartUp:
            audioPlayer(startUp, 4);
            break;
        case GameOver:
            audioPlayer(gameOver, 5);
            break;
        case GameWin:
            audioPlayer(gameWin, 4);
            break;
        case MoleUp:
            audioPlayer(moleUp, 2);
            break;
        case MoleDown:
            audioPlayer(moleDown, 2);
            break;
        case HammerHit:
            audioPlayer(hammerHit, 2);
            break;
        case HammerMiss:
            audioPlayer(hammerMiss, 2);
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
    enablePWM();

    if (!firstNoteStartTimeIsSet) {
        note_start_time = *timer1_overflow_count;
        firstNoteStartTimeIsSet = true;
    }

    // Check if the current note is complete
    if (*timer1_overflow_count - note_start_time >= sound_array[current_note].duration) {
        current_note++;

        if (current_note >= sound_array_length) {
            is_playing_sound = false;  // Stop playback
            disablePWM();
            return;
        }
        note_start_time = *timer1_overflow_count;  // Start the next note
    }

    // Update PWM for the current note
    uint8_t ocrTop = freqToOCRTop(sound_array[current_note].note);
    OCR2A = ocrTop;
    OCR2B = ocrTop / 2;  // Set duty cycle to 50%
}

void Audio::playSound(Sound sound) {
    if (is_playing_sound) {
        return;
    }

    // reset associated variables
    current_note = 0;
    remaining_note_time = 0;
    note_start_time = 0;
    is_playing_sound = true;
    firstNoteStartTimeIsSet = false;

    switch (sound)
    {
        case ThemeSong0:
            current_sound = ThemeSong0;
            break;

        case ThemeSong1:
            current_sound = ThemeSong1;
            break;

        case MoleUp:
            current_sound = MoleUp;
            break;

        case MoleDown:
            current_sound = MoleDown;
            break;

        case HammerHit:
            current_sound = HammerHit;
            break;

        case HammerMiss:
            current_sound = HammerMiss;
            break;

        case GameOver:
            current_sound = GameOver;
            break;

        case GameWin:
            current_sound = GameWin;
            break;

        case ButtonPress:
            current_sound = ButtonPress;
            break;

        case StartUp:
            current_sound = StartUp;
            break;
    }
}