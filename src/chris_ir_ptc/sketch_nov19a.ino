#define RX 2
#define TX 6
#define BIT_DURATION 100  // microseconds

volatile unsigned long pulseWidth = 0;
volatile bool pulseDetected = false;

void setup() {
  pinMode(RX, INPUT);
  pinMode(TX, OUTPUT);
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(RX), measurePulse, CHANGE);
}

int end_millis = 0;
bool started = false;

void loop() {
  digitalWrite(13, digitalRead(RX));  // CONNECT LED TO 13 FOR DEBUGGING
  if (pulseDetected) {
    noInterrupts();
    unsigned long currentPulseWidth = pulseWidth;
    pulseDetected = false;

    if (currentPulseWidth > 0) {
      decodePulse(currentPulseWidth);
    } 
    interrupts();
  }

  // Start and stop the 38 kHz burst for 5 ms
  if (!started) {
    end_millis = millis() + 5;
    if (millis() > end_millis) {
      start38kHz();
      started = true;
    }
  } else {
    end_millis = millis() + 5;
    if (millis() > end_millis) {
      stop38kHz();
      started = false;
    }
  }
}

void measurePulse() {
  // measure the length of every LOW pulse
  static unsigned long startTime = 0;

  if (digitalRead(RX) == LOW) {
    startTime = micros();
  } else {
    pulseWidth = micros() - startTime;
    pulseDetected = true;
  }
}

void decodePulse(int pulse) {
  uint32_t cmd = 0;

  int pulseWidthBitsAmt = pulse / BIT_DURATION;

  for (int i = 0; i <= pulseWidthBitsAmt; i++) {
    cmd++;
    
    if (i != pulseWidthBitsAmt) {
      cmd <<= 1;
    }
  }

  Serial.println(cmd, BIN);
}

void start38kHz() {
  tone(TX, 38000);
}

void stop38kHz() {
  noTone(TX);
}
