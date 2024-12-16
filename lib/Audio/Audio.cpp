#include <Audio.h>
#include <avr/io.h>

Audio::Audio()
    : timer1_overflow_count{0}
    , current_sound{0}
    , current_note{0}
    , remaining_note_time{0}
    , is_playing_sound{false}

    , buttonPress{C5, 4}
{
}

uint8_t Audio::freqToOCRTop(uint16_t freq) {
    return F_CPU / (2 * 256 * freq) - 1;
}

void Audio::setTimingVariable(uint32_t* timer1_overflow_count){
    this->timer1_overflow_count = timer1_overflow_count;
}

void Audio::init(){
    // Set pin 3 as output for the speaker
    DDRD |= (1 << PD3);

    // Set up Timer2 for PWM
    TCCR2A = (1 << WGM21) | (1 << COM2B0);  // CTC mode, toggle OC2B on compare match
    TCCR2B = (1 << CS22) | (1 << CS20);     // Prescaler of 64
}

void Audio::handleTimer1ISR() {
    if (!is_playing_sound) {
        TCCR2A &= ~(1 << COM2B0); // Turn off PWM
        PORTD &= ~(1 << PD3); // Turn off the speaker
        return;
    }

    switch (current_sound) {
        case ButtonPress:
            player(buttonPress, 2);
            break;
    }
}

void Audio::player(uint16_t sound_array[], uint8_t length) {
    // Play the note
    TCCR2A |= (1 << COM2B0); // Turn on PWM
    OCR2B = freqToOCRTop(sound_array[current_note]);
    remaining_note_time = sound_array[current_note] - (*timer1_overflow_count - note_start_time);

    // Go to next note if the current note is done
    if (remaining_note_time == 0) {
        note_start_time = *timer1_overflow_count;
        current_note += 2;

        // Stop playing the sound if the last note is done
        if (current_note == 1) {
            is_playing_sound = false;
            return;
        }
    }

    else remaining_note_time--;
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
    }
        
}