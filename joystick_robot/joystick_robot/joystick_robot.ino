// ME350R Joystick Robot
// Elliott Turner
// Feb, 2023

// deadzone
#define D_ZONE 0.05

// pin definitions
#define ENA 9
#define ENB 10
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7
#define JX A0
#define JY A1

void setup() {
  // set all pins as output
  int pins[] = {ENA, ENB, IN1, IN2, IN3, IN4};
  for (int i=0; i<sizeof(pins)/sizeof(pins[0]); i++) { pinMode(pins[i], OUTPUT); }

  Serial.begin(9600);
}

void loop() {
  float x = (float)(analogRead(JX)-512) / 512.0;
  float y = (float)(analogRead(JY)-512) / 512.0;
  update_motors(-y, x);
}

void update_motors(float fwd, float trn) {
  // fwd: 1=forward, -1=backward; trn: 1=right, -1=left
  float l = fwd + trn;
  float r = fwd - trn;
  if (l>1) { l=1; }
  if (l<-1) { l=-1; }
  if (r>1) { r=1; }
  if (r<-1) { r=-1; }
  if (abs(l) <= D_ZONE) { l = 0; }
  if (abs(r) <= D_ZONE) { r = 0; }
  update_motor(ENA, IN1, IN2, l);
  update_motor(ENB, IN3, IN4, r);
}

void update_motor(int en, int in1, int in2, float speed) {
  digitalWrite(in1, speed<0);
  digitalWrite(in2, speed>0);
  analogWrite(en, abs(speed) * 255);
}
