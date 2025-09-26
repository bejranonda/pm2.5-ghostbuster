#include <Arduino.h>
#include "MHZ19.h"                                        
#include <SoftwareSerial.h>                                // Remove if using HardwareSerial

#define RX_PIN 14                                          // D5 14 von WEMOS Mini Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 12                                          // D6 12 von WEMOS Mini Tx pin which the MHZ19 Rx pin is attached to
//#define RX_PIN D5                                          // D7 von WEMOS Mini Rx pin which the MHZ19 Tx pin is attached to
//#define TX_PIN D6                                          // D8 von WEMOS Mini Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)
#define UPDATE_MS 5000                                      // Device to MH-Z19 Serial baudrate (should not be changed)

#define SCHWELLE1 1500 //Schwellwert wenn die LED von Grün auf Rot wechseln soll.
#define SCHWELLE2 2500 //Schwellwert wenn die LED von Grün auf Rot wechseln soll.

#define sample 64                    //300 this is how many samples the device takes per reading
#define record_at_loop 10  // record every xx loops

#define plot_interval 44  // (2500-400)/48 each 44 ppm is one dot

byte array_val[sample];                      //creates an array with number of elements equal to "sample"
unsigned long getDataTimer = 0;

MHZ19 myMHZ19;                                             // Constructor for library
SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial


///////
// OLED
///////

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

  
void setup()
{
    Serial.begin(9600);                                     // Device to serial monitor feedback

    mySerial.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start   
    myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin(). 

    myMHZ19.autoCalibration();                              // Turn auto calibration ON (OFF autoCalibration(false))

  ///////
  // OLED
  //////
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  // init done

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  // miniature bitmap display
  display.clearDisplay();
  display.setRotation(2); // rotate upside down
  display.setTextSize(3);
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.setCursor(0,0);
  display.println(F("mod"));
  display.setTextSize(2);
  display.setTextColor(WHITE); // 'inverted' text
  display.setCursor(2,30);
  display.println(F("child"));
  display.display();
  // Clear the buffer.


}

byte sample_count = 0;
byte record_count = 0; 

void loop()
{

    if (millis() - getDataTimer >= UPDATE_MS)
    {
        int CO2; 

        /* note: getCO2() default is command "CO2 Unlimited". This returns the correct CO2 reading even 
        if below background CO2 levels or above range (useful to validate sensor). You can use the 
        usual documented command with getCO2(false) */

        CO2 = myMHZ19.getCO2();                             // Request CO2 (as ppm)
        
        Serial.print("CO2 (ppm): ");                      
        Serial.println(CO2);                                

        //int8_t Temp = myMHZ19.getTemperature();                     // Request Temperature (as Celsius)
        float Temp = myMHZ19.getTemperature();                     // Request Temperature (as Celsius)
        Serial.print("Temperature (C): ");                  
        Serial.println(Temp);                               
        
        display.clearDisplay();

        display.setRotation(2); // rotate upside down
        display.setTextColor(WHITE);
        if(CO2 > SCHWELLE1){
          display.setTextSize(3);
          Serial.println(F("Schwelle1"));   
        }else if(CO2 > SCHWELLE2){
          display.setTextSize(3);
          display.setTextColor(BLACK, WHITE); // 'inverted' text
          Serial.println(F("Schwelle2"));   
        }else{
          display.setTextSize(2);
          display.setTextColor(WHITE);
          Serial.println(F("Normal"));   
        }
        display.setCursor(2,2);
        
//        display.print("CO2");
        display.println(CO2);
        display.println();
        display.setTextSize(1);
//        display.setTextColor(WHITE);
        display.print(Temp,1);
        display.println(" C");
        display.display();

        
        // Plot
        array_val[sample_count] = CO2/plot_interval;
        delay(500);
        byte count_plot = 0;
        byte plot_i = sample_count+1;
//          Serial.println(sample_count);   
        
        while(count_plot < sample-1){
          byte plot_val = array_val[plot_i];
          display.drawPixel(count_plot, 64-plot_val, WHITE);
//            display.drawPixel(count_plot*2, plot_val, WHITE);
//            display.drawPixel(count_plot*2+1, plot_val, WHITE);
          display.display();

//            Serial.print(count_plot);   
//            Serial.print("-");   
//            Serial.print(plot_val/50);   
//            Serial.print(",");   
          count_plot++;
          plot_i++;
          if(plot_i >= sample){
            plot_i = 0;
          }
          delay(1);
        }
        delay(10);
//          Serial.println(F("END Plot"));   

        if(record_count >= record_at_loop){
          if(sample_count >= sample - 1){
            sample_count = 0;
          }else{
            sample_count++;
          }
          record_count = 0;
          
        }else{
          record_count++;
        }
        
        getDataTimer = millis();
    }
}
