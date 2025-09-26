// Aaron ALAI EMF Detector May 28th 2009 VERSION 1.1
// aaronalai1@gmail.com
// contains code for averaging sensor data

#define sample 5000                    //300 this is how many samples the device takes per reading
                                                        //more information for #define http://arduino.cc/en/Reference/Define
int senseLimit = 20; // raise this number to decrease sensitivity (up to 1023 max)
int inPin = 5;                                  //analog 5
float val;                                         //where to store info from analog 5
int pin11 = 11;                              //output of red led

//int array1[sample];                      //creates an array with number of elements equal to "sample"
                                                       //more information about arrays http://arduino.cc/en/Reference/Array


//unsigned long averaging;         //the program uses this variable to store the sum of each array it makes
unsigned long sum;         //the program uses this variable to store the sum of each array it makes
int val_max = 0;
int val_min = 255;


void setup() {
 
  //Serial.begin(115200);
  Serial.begin(9600);
 
}

void loop() {

  int sample_count = 0;
  for(int i = 0; i < sample; i++){              //this code tells the program to fill each element in the array we made with
     //array1[i] = analogRead(inPin);       //information from the antenna wire coming out of the Arduino
     //averaging += array1[i];                      //more information about for loops http://arduino.cc/en/Reference/For
     int val_read = analogRead(inPin);
     if(val_read > 0){
       //val_read = constrain(val_read, 1, senseLimit);  // turn any reading higher than the senseLimit value into the senseLimit value
       //val_read = map(val_read, 1, senseLimit, 1, 1023);  // remap the constrained value within a 1 to 1023 range
       sum += val_read;
       if(val_read > val_max){
          //Serial.println("Max");
          val_max = val_read;
       }else if(val_read < val_min){
          //Serial.println("Min");
          val_min = val_read;
       }
       sample_count++;
       // delay(1);
     }
  }                                                               //the averaging line is simply saying: add averaging to whatever is in array position i
                                                                  //averaging += array[i] is the same as averaging = averaging + array[i]
                                                                  //for more information about += http://arduino.cc/en/Reference/IncrementCompound
  
  
  
  val = float(sum)/sample_count;                   //here the program takes the sum of all numbers in array1, and divides by the number of elements "sample"
  //val = averaging/sample;                   //here the program takes the sum of all numbers in array1, and divides by the number of elements "sample"
  
  //val = constrain(val, 0, 100);               //this constrains the variable value to between two numbers 0 and 100                                      
  //val = map(val, 0, 100, 0, 255);      //for more information about constrain http://arduino.cc/en/Reference/Constrain  
  //analogWrite(pin11, val);                //the map statement tells the program to map out 0-100 to 0-255, 255 is
                                                            //the threashold of analogWrite for more information about map http://arduino.cc/en/Reference/Map
  //averaging = 0;                                 //this line of code sets averaging back to zero so it can be used again
  //Serial.println(String(val) + "," + String(val_max) + "," + String(val_min) + "," + String(sample_count));                                // use output to aid in calibrating
  Serial.println(String(val) + "," + String(val_max) + "," + String(val_min));                                // use output to aid in calibrating
  sum = 0;
  val_max = 0;
  val_min = 255;
  
  delay(10);
}
