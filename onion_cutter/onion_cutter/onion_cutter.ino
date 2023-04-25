// ME350R Joystick Robot
// Elliott Turner
// Apr, 2023

// pin definitions
#define ENA 9
#define IN1 10
#define IN2 11
#define ENC_A 2
#define ENC_B 3
#define BUTTON 8

// 12 PPR encoder and 1/98 MA -> 1176 PPR
// 1176 * 36/12 = 3528 pulses for complete rotation of *both* outputs
#define PPR 3528
#define OVERSHOOT 0
#define CUT_COUNT 5

#define K_p 50.0
#define K_i 0.0005
#define K_d 0.01
#define V_ref 8.5

float P_error;
float I_error;
float D_error;

unsigned long position; // [encoder pulses]
float velocity; // [encoder pulses per second]
unsigned long last_position;
unsigned long last_time;

void setup() {
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENC_A, INPUT);
    pinMode(ENC_B, INPUT);
    pinMode(BUTTON, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(ENC_A), encoder_isr, RISING);

    // set motor direction (does not change) and speed to 0
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(ENA, LOW);

    position = 0;

    Serial.begin(9600);
}

int counter = 0;
void loop() {
    if (!digitalRead(BUTTON)) {
        if (++counter > 10000) {
          counter = 0;
          // feedback control
          unsigned long current_time = micros();
          float delta_time = (float)current_time - (float)last_time;
          last_time = current_time;
          float delta_position = (float)position - (float)last_position;
          last_position = position;

          velocity = ((delta_position*10000) / delta_time)*0.4 + 0.6*velocity;

          float error = V_ref - velocity; 

          D_error = (error - P_error) / delta_time;
          P_error = error;
          I_error += P_error * delta_time;

          if (I_error > 1000000) { I_error = 1000000; }
          if (I_error < -1000000) { I_error = -1000000; }
          
          int response = P_error * K_p + I_error * K_i + D_error * K_d;
          if (response > 255) { response = 255; }
          if (response < 0) { response = 0; }
          if (position < PPR * CUT_COUNT - OVERSHOOT) { analogWrite(ENA, response); }

          Serial.print("V_ref:");
          Serial.print(V_ref);
          Serial.print(",V:");
          Serial.print(velocity);
          Serial.print(",error:");
          Serial.print(error);
          Serial.print(",response:");
          Serial.println(response/255.0);
        }
    }
    else {
        digitalWrite(ENA, LOW);
        position = 0;
        P_error = 0;
        I_error = 0;
        D_error = 0;
        last_time = micros();
    }

    
}

void encoder_isr(void)
{
    position++;
    if (position >= PPR * CUT_COUNT - OVERSHOOT) { digitalWrite(ENA, LOW); }
}
