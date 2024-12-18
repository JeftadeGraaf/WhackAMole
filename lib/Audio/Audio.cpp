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
        NoteDuration{C5, 4}
        } 
    , startUp{
        NoteDuration{C4, 4},
        NoteDuration{E4, 4},
        NoteDuration{G4, 4},
        NoteDuration{C5, 4}
    }
{
}

uint8_t Audio::freqToOCRTop(uint16_t freq) {
    return (F_CPU / (2 * 64 * freq)) - 1;
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