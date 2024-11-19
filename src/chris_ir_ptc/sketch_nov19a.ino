#define RX 2
#define TX 6

void setup() {
  // put your setup code here, to run once:
  pinMode(RX, INPUT);
  Serial.begin(9600);
  pinMode(TX, OUTPUT);
}

void loop() {
    // Generate a 38 kHz square wave burst for 5 ms
  tone(TX, 38000);  // Start 38 kHz signal on pin TX
  delay(1);         // Duration of the burst

  // put your main code here, to run repeatedly:
    int pulseWidth = pulseIn(RX, LOW, 100000);

    if (pulseWidth > 0) {
      Serial.println(pulseWidth);
    }

  noTone(TX);       // Stop the signal
  delay(1);         // Wait 5 ms before the next burst
}
