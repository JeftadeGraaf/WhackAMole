#include <Audio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <HardwareSerial.h>

#define PRESCALER_64_THRESHOLD 488

Audio::Audio(Timer1Overflow &timer1)
    : current_sound{0}, current_note{0}, is_playing_sound{false}, firstNoteStartTimeIsSet{false}

      // note, duration [in 32ms ticks]
      ,
      startUp{
          NoteDuration{C4, NOTELENGTH_SHORT},
          NoteDuration{E4, NOTELENGTH_SHORT},
          NoteDuration{G4, NOTELENGTH_SHORT},
          NoteDuration{C5, NOTELENGTH_SHORT}},
      gameOver{
          NoteDuration{G3, NOTELENGTH_MEDIUM},
          NoteDuration{FS3, NOTELENGTH_MEDIUM},
          NoteDuration{F3, NOTELENGTH_MEDIUM},
          NoteDuration{E3, NOTELENGTH_MEDIUM},
          NoteDuration{DS3, NOTELENGTH_MEDIUM}},
      gameWin{
          NoteDuration{C4, NOTELENGTH_MEDIUM},
          NoteDuration{E4, NOTELENGTH_MEDIUM},
          NoteDuration{G4, NOTELENGTH_MEDIUM},
          NoteDuration{C5, NOTELENGTH_LONG}},
      moleUp{
          NoteDuration{C4, NOTELENGTH_SHORT},
          NoteDuration{G4, NOTELENGTH_SHORT}},
      hammerHit{
          NoteDuration{G4, NOTELENGTH_SHORT},
          NoteDuration{C5, NOTELENGTH_SHORT}},
      hammerMiss{
          NoteDuration{C5, NOTELENGTH_SHORT},
          NoteDuration{FS4, NOTELENGTH_SHORT}}

      ,
      themeSong0{
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
          NoteDuration{REST, NOTELENGTH_MEDIUM}}
{
    this->timer1 = &timer1;
}

void setPrescaler1()
{
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS20);
}

void setPrescaler8()
{
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS21);
}

void setPrescaler32()
{
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS21) | (1 << CS20);
}

void setPrescaler64()
{
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS22);
}

void setPrescaler128()
{
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS22) | (1 << CS20);
}

void setPrescaler256()
{
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS22) | (1 << CS21);
}

void setPrescaler1024()
{
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
}

// Function that returns the OCR value for a given frequency and sets the prescaler accordingly
uint8_t Audio::freqToOCRTop(uint16_t freq)
{
    // Available prescaler values for Timer2
    const uint16_t prescalerValues[] = {1, 8, 32, 64, 128, 256, 1024};
    const uint8_t prescalerCount = sizeof(prescalerValues) / sizeof(prescalerValues[0]);

    uint8_t bestOcrTop = 0;
    uint16_t bestPrescaler = 0;

    // Iterate over the available prescaler values to find the best prescaler and OCR value
    for (uint8_t i = 0; i < prescalerCount; i++)
    {
        uint16_t prescaler = prescalerValues[i];
        uint32_t ocrTop = (F_CPU / (2UL * prescaler * freq)) - 1;

        // Check if ocrTop fits in 8 bits
        if (ocrTop <= 255)
        {
            // Prefer higher ocrTop value for better resolution
            if (ocrTop > bestOcrTop)
            {
                bestOcrTop = ocrTop;
                bestPrescaler = prescaler;
            }
        }
    }

    // If a suitable prescaler was found, set it
    if (bestPrescaler != 0)
    {
        switch (bestPrescaler)
        {
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
    }
    else
    {
        // If no suitable prescaler was found, use the highest prescaler value
        setPrescaler1024();
        bestOcrTop = 255;
    }

    return bestOcrTop;
}

void Audio::init()
{
    // Set pin 3 as output for the speaker
    DDRD |= (1 << PD3);

    // Set up Timer2 for PWM
    TCCR2A = (1 << WGM21) | (1 << WGM20) | (1 << COM2B1); // Fast PWM, clear OC2B on compare match
    TCCR2B = (1 << WGM22) | (1 << CS22) | (1 << CS20);    // Fast PWM with prescaler of 64
}

void Audio::handleTimer1ISR()
{
    if (!is_playing_sound)
    {
        disablePWM();
        return;
    }

    switch (current_sound)
    {
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
    case HammerHit:
        audioPlayer(hammerHit, hammerHit_LENGTH);
        break;
    case HammerMiss:
        audioPlayer(hammerMiss, hammerMiss_LENGTH);
        break;
    case ThemeSong0:
        audioPlayer(themeSong0, themeSong0_LENGTH);
        break;
    }
}

void Audio::enablePWM()
{
    TCCR2A |= (1 << COM2B0) | (1 << COM2B1);
}

void Audio::disablePWM()
{
    TCCR2A &= ~((1 << COM2B0) | (1 << COM2B1));
    PORTD &= ~(1 << PD3); // Turn off the speaker for good measure
}

void Audio::audioPlayer(NoteDuration *sound_array, uint8_t sound_array_length)
{
    if (!firstNoteStartTimeIsSet)
    {
        note_start_time = timer1->overflowCount;
        firstNoteStartTimeIsSet = true;
    }

    // Check if the current note is complete
    if (timer1->overflowCount - note_start_time >= sound_array[current_note].duration)
    {
        current_note++;

        if (current_note >= sound_array_length)
        {
            stopSound();
            return;
        }
        note_start_time = timer1->overflowCount; // Start the next note
    }

    // Check if current note is a REST
    if (sound_array[current_note].note == REST)
    {
        disablePWM();
        return;
    }

    // Only enable PWM for non-REST notes
    enablePWM();
    uint8_t ocrTop = freqToOCRTop(sound_array[current_note].note);
    OCR2A = ocrTop;
    OCR2B = ocrTop / PWM_DUTY_CYCLE_DIVIDER; // Set duty cycle to 50%
}

void Audio::playSound(Sound sound)
{
    if (is_playing_sound)
    {
        return;
    }

    // reset associated variables
    current_note = 0;
    note_start_time = 0;
    is_playing_sound = true;
    firstNoteStartTimeIsSet = false;

    current_sound = sound;
}

void Audio::stopSound()
{
    is_playing_sound = false;
    disablePWM();
}

void Audio::test_one_by_one()
{
    while (1)
    {
        _delay_ms(1000);
        playSound(StartUp);
        _delay_ms(1000);
        playSound(GameOver);
        _delay_ms(1000);
        playSound(GameWin);
        _delay_ms(1000);
        playSound(MoleUp);
        _delay_ms(1000);
        playSound(HammerHit);
        _delay_ms(1000);
        playSound(HammerMiss);
    }
}