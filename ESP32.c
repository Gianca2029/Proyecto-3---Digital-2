#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_NeoPixel.h>

MPU6050 imu;                      // sensor I2C
HardwareSerial link(2);           // UART2 para STM32

const int PIN_NEO = 12;           // neopixel
const int NLED   = 8;

Adafruit_NeoPixel ring(NLED, PIN_NEO, NEO_GRB + NEO_KHZ800);

int16_t ax, ay, az;
int16_t gx, gy, gz;
uint8_t ultimoValor = 0;

uint8_t limitar(uint8_t v) {      // limita a 0..9
  if (v > 9) return 9;
  return v;
}

void colorNeo(uint32_t c) {       // enciende todos los LEDs
  for (int i = 0; i < NLED; i++) ring.setPixelColor(i, c);
  ring.show();
}

void leerSensor() {               // lee aceleración y giro
  imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
}

uint8_t mapearValor(int16_t v) {  // mapea -32768..32767 a 0..9
  long r = map(v, -32768, 32767, 0, 9);
  if (r < 0) r = 0;
  if (r > 9) r = 9;
  return (uint8_t)r;
}

void setup() {
  Serial.begin(115200);           // monitoreo
  Wire.begin();
  imu.initialize();

  ring.begin();
  ring.clear();
  ring.setBrightness(180);
  colorNeo(ring.Color(10, 0, 10)); // arranque

  link.begin(115200, SERIAL_8N1, 16, 17); // RX=16, TX=17
}

void loop() {
  if (link.available()) {
    char cmd = link.read();

    if (cmd == 'R') {             // solicita lectura
      leerSensor();
      uint8_t v = mapearValor(ax);
      ultimoValor = v;
      link.write(v);
    } else {                      // comando de color
      switch (cmd) {
        case '1':
          colorNeo(ring.Color(255, 0, 0)); // rojo
          break;
        case '2':
          colorNeo(ring.Color(0, 255, 0)); // verde
          break;
        case '3':
          colorNeo(ring.Color(0, 0, 255)); // azul
          break;
        default:
          colorNeo(ring.Color(20, 20, 0)); // estado neutro
          break;
      }
    }
  }

  delay(20);
}
