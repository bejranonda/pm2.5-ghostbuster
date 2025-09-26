#include <Melopero_AMG8833.h>

Melopero_AMG8833 sensor;

void print_val(float val) {
  Serial.print(val);
  Serial.print(",");
  
}

void setup() {
//  Serial.begin(9600);
  Serial.begin(57600);

  // initializing I2C to use default address AMG8833_I2C_ADDRESS_B and Wire (I2C-0):
  Wire.begin();
  sensor.initI2C();
  // To use Wire1 (I2C-1):
  // Wire1.begin();
  // sensor.initI2C(AMG8833_I2C_ADDRESS_B, Wire1);

//  Serial.print("Resetting sensor ... ");  
  int statusCode = sensor.resetFlagsAndSettings();
//  Serial.println(sensor.getErrorDescription(statusCode));

//  Serial.print("Setting FPS ... ");
//  statusCode = sensor.setFPSMode(FPS_MODE::FPS_10);
  statusCode = sensor.setFPSMode(FPS_MODE::FPS_1);
//  Serial.println(sensor.getErrorDescription(statusCode));
}

void loop() {
//  Serial.print("Updating thermistor temperature ... ");
//  int statusCode = sensor.updateThermistorTemperature();
//  Serial.println(sensor.getErrorDescription(statusCode));

//  Serial.print("Updating pixel matrix ... ");
  int statusCode = sensor.updatePixelMatrix();
//  Serial.println(sensor.getErrorDescription(statusCode));

//  Serial.print("Thermistor temp: ");
//  Serial.print(sensor.thermistorTemperature);
//  Serial.println("°C");

//  Serial.println("Temperature Matrix: ");
  for (int x = 0; x < 8; x++){
    for (int y = 0; y < 8; y++){
//      Serial.print(sensor.pixelMatrix[y][x]);
//      Serial.print(",");
      print_val(sensor.pixelMatrix[y][x]);
    }
//    Serial.println();
  }

  Serial.println();
  delay(100);
}
