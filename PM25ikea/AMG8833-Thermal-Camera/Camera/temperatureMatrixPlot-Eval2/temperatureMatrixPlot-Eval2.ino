#include <Melopero_AMG8833.h>

Melopero_AMG8833 sensor;

const int event_loop_limit = 100; // Number of loop for an event
float temp_matrix[64];
//float temp_prv[64];

//int event_loop_i = 0; // Number of loop

void print_val(float val) {
  Serial.print(val);
  Serial.print(F(","));
  
}

void setup() {
  //Serial.begin(9600);
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
  float event_loop_max = -100;
  float event_loop_min = 100;
  float event_dif_positive_max = -100;
  float event_dif_negative_min = 100;

  byte loop_status = 0;
  // 0 = no report
  // 1 = this_loop_min decrease lower than min_thershold
  // 2 = this_loop_max increase greater than max_thershold
  // 3 = this_dif_negative_min decrease lower than negative_dif_thershold
  // 4 = this_dif_postive_max increase greater than positive_dif_thershold

  // 5 = same as 1, but extrem than factor extreme_lower_thershold
  // 6 = same as 2, but extrem than factor extreme_upper_thershold
  // 7 = same as 3, but extrem than factor extreme_upper_thershold
  // 8 = same as 4, but extrem than factor extreme_upper_thershold
  
  // 9 = abs_deviation > abs_deviation_thershold // temperature change much by pexel 
  // 10 = abs_deviation > abs_deviation_thershold and > abs(deviation)*moving_deviation_factor // temperature change much by pexel, but all inside camera doesn't change so much // moving

  const float min_thershold = 0.95;
  const float max_thershold = 1.05;
  const float negative_dif_thershold = 1.05;
  const float positive_dif_thershold = 1.05;
  const float extreme_lower_thershold = 0.7;
  const float extreme_upper_thershold = 1.3;
  
  const float abs_deviation_thershold = 40; //temperature differnce frome previous reading
  const float moving_deviation_factor = 0.5; //less means the temperature inside camera doesn't change so much
  
  const float intial_pos = 100;
  const float intial_neg = -100;
  const int camera_pexels = 8;

  boolean first_loop = true;

  while(true){
    for (int event_loop_i = 0; event_loop_i < event_loop_limit; event_loop_i++){

    float this_loop_max = intial_neg;
    float this_loop_min = intial_pos;
    float this_dif_postive_max = intial_neg;
    float this_dif_negative_min = intial_pos;
    
    if(event_loop_i == 0){
      event_loop_max = event_loop_max*0.8;
      event_loop_min = event_loop_min*1.2;
      event_dif_positive_max = event_dif_positive_max*0.8;
      event_dif_negative_min = event_dif_negative_min*0.8;
      loop_status = 0;
      //Serial.println("CLEARRR");
    }
  
  
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

      float temp_dif;
      float deviation = 0;
      float abs_deviation = 0;
      
      for (int x = 0; x < camera_pexels; x++){
        for (int y = 0; y < camera_pexels; y++){
          int i = x*camera_pexels + y;
          float read_i = sensor.pixelMatrix[y][x];
    
          // Max Min
          if(read_i > this_loop_max){
              this_loop_max = read_i;
          }
    
          if(read_i < this_loop_min){
              this_loop_min = read_i;
          }
          
          // Compare with previous
          temp_dif =  read_i - temp_matrix[i];
          deviation = temp_dif + deviation;
          abs_deviation = abs(temp_dif) + abs_deviation;

          if(temp_dif > 0){
            if(temp_dif > this_dif_postive_max){
              this_dif_postive_max = temp_dif;  
            }
          }else{
            if(temp_dif < this_dif_negative_min){
                this_dif_negative_min = temp_dif;
            }
          }
    
          
          // Record
          temp_matrix[i] = read_i;
    //      Serial.print(read_i);
          //Serial.print(sensor.pixelMatrix[y][x]);
    //      Serial.print(",");
          print_val(read_i);
          
        }
    //    Serial.println();
      }
    
    //  Serial.println();
    //  Serial.println("This Loop");
    //  Serial.println(this_loop_min);
    //  Serial.println(this_loop_max);
    //  Serial.println(this_dif_negative_min);
    //  Serial.println(this_dif_postive_max);
    //  
            
      // Compare Max Min for event
   
      if(this_loop_min < event_loop_min){
        if(this_loop_min < event_loop_min*min_thershold){
          if(this_loop_min < event_loop_min*min_thershold*extreme_lower_thershold){
            loop_status = 5;  
          }else{
            loop_status = 1;  
          }
        }
        event_loop_min = this_loop_min;
    //      Serial.println("MnE");
      }

      if(this_loop_max > event_loop_max){
        if(this_loop_max > event_loop_max*max_thershold){
          if(this_loop_max > event_loop_max*max_thershold*extreme_upper_thershold){
            loop_status = 6;  
          }else{
            loop_status = 2;  
          }
        }
        event_loop_max = this_loop_max;
    //      Serial.println("MxE");
      }


    
      if(this_dif_negative_min < event_dif_negative_min){
        if(this_dif_negative_min < event_dif_negative_min*negative_dif_thershold){
//          Serial.println();
//          Serial.println(this_dif_negative_min);
//          Serial.println(event_dif_negative_min);
          if(this_dif_negative_min < event_dif_negative_min*negative_dif_thershold*extreme_upper_thershold){
            loop_status = 7;  
          }else{
            loop_status = 3;  
          }
        }
        event_dif_negative_min = this_dif_negative_min;
    //      Serial.println("DnE");
      }


      
      // Compare Dif for event
      if(this_dif_postive_max > event_dif_positive_max){
        if(this_dif_postive_max > event_dif_positive_max*positive_dif_thershold){
          if(this_dif_postive_max > event_dif_positive_max*positive_dif_thershold*extreme_upper_thershold){
            loop_status = 8;  
          }else{
            loop_status = 4;  
          }
        }
        event_dif_positive_max = this_dif_postive_max;
    //      Serial.println("DxE");
      }


    if(abs_deviation > abs_deviation_thershold){
      if(abs_deviation*moving_deviation_factor > abs(deviation)){
        loop_status = 10;
      }else{
        if(loop_status < 2){
          loop_status = 9;  
        }
      }
    }
      
      
    
    //  
    //  Serial.println("This Event");
    //  Serial.println(event_loop_min);
    //  Serial.println(event_loop_max);
    //  Serial.println(event_dif_negative_min);
    //  Serial.println(event_dif_positive_max);

//      Serial.println();

      if(event_loop_i == 0 || first_loop){
        loop_status = 0;
      }
      
      print_val(loop_status);
//      print_val(this_loop_min);
//      print_val(this_loop_max);
//      print_val(this_dif_negative_min);
//      print_val(this_dif_postive_max);
//
//      print_val(event_loop_min);
//      print_val(event_loop_max);
//      print_val(event_dif_negative_min);
//      print_val(event_dif_positive_max);
//      print_val(event_loop_i);
//      print_val(first_loop);
//      print_val(deviation);
//      print_val(abs_deviation);
      
      Serial.println();
//      Serial.println(event_loop_i);

      if(event_loop_i > event_loop_limit-2){
        first_loop = false;
      }
      delay(100);
    }

  }
}
