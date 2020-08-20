const int SERIAL_DATA = 14;
const int SERIAL_CLOCK = 10;
const int OUT_DISABLE = 16;
const int DISABLE_DISP_1 = 9;
const int DISABLE_DISP_2 = 8;
const int THERMISTOR = A0;

void setup() {
  pinMode(SERIAL_DATA, OUTPUT);
  pinMode(SERIAL_CLOCK, OUTPUT);
  pinMode(OUT_DISABLE, OUTPUT);
  pinMode(DISABLE_DISP_1, OUTPUT);
  pinMode(DISABLE_DISP_2, OUTPUT);
  pinMode(THERMISTOR, INPUT);

  digitalWrite(OUT_DISABLE, HIGH);
  digitalWrite(DISABLE_DISP_1, HIGH);
  digitalWrite(DISABLE_DISP_2, HIGH);
}

const double A = -16.04862873 * 0.001;
const double B = 29.39520156 * 0.0001;
const double C = -97.76677569 * 0.0000001;
int getTemperature(){
  float volt = analogRead(THERMISTOR) / 1024.0 * 5;
  float res = 10000 * (5 / volt - 1);
  float lnres = log(res);
  int temp = 1 / (A + B * lnres + C * pow(lnres, 3)) - 272.65;
  return temp;
}

const int CLOCK_SPEED = 1;
void sendByte(byte data){
  for (int i = 0; i < 8; i ++){
    byte b = (data >> i) & 1;
    digitalWrite(SERIAL_DATA, b);
    digitalWrite(SERIAL_CLOCK, HIGH);
    delayMicroseconds(CLOCK_SPEED);
    digitalWrite(SERIAL_CLOCK, LOW);
    digitalWrite(SERIAL_DATA, LOW);
    delayMicroseconds(CLOCK_SPEED);
  }
}

const int DISP_ON_TIME = 500;
void flash(int data){
  sendByte(data >> 8);
  digitalWrite(DISABLE_DISP_1, LOW);
  digitalWrite(OUT_DISABLE, LOW);
  delayMicroseconds(DISP_ON_TIME);
  digitalWrite(OUT_DISABLE, HIGH);
  digitalWrite(DISABLE_DISP_1, HIGH);
  sendByte(data);
  digitalWrite(DISABLE_DISP_2, LOW);
  digitalWrite(OUT_DISABLE, LOW);
  delayMicroseconds(DISP_ON_TIME);
  digitalWrite(OUT_DISABLE, HIGH);
  digitalWrite(DISABLE_DISP_2, HIGH);
}

const byte SEGMENT [10] = {
  0b11111100,
  0b00110000,
  0b11011010,
  0b01111010,
  0b00110110,
  0b01101110,
  0b11101110,
  0b00111000,
  0b11111110,
  0b01111110,
};
int render(byte num){
  byte ten = num / 10;
  byte one = num % 10;
  int data = SEGMENT[ten] << 8 | SEGMENT[one];
  return data;
}

const int PERIOD = 1000;
int temp;
int data;
int counter = 0;
void loop() {
  if (!counter){
    temp = getTemperature();
    data = render(temp);
    counter = PERIOD;
  }
  flash(data);
  counter --;
}
