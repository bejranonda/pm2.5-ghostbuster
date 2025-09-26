// Use the MD_MAX72XX library to scroll text on the display
//
// Demonstrates the use of the callback function to control what
// is scrolled on the display text.
//
// User can enter text on the serial monitor and this will display as a
// scrolling message on the display.
// Speed for the display is controlled by a pot on SPEED_IN analog in.
//

// Usage
// Key in on serial
// test -- print test with default delay 5
// test,9 -- print test with delay 9
// test,p -- show pacman then print test with default delay 5
// test,p9 -- show pacman then print test with delay 9

#include <MD_MAX72xx.h>
#include <SPI.h>

#define IMMEDIATE_NEW   0     // if 1 will immediately display a new message
#define USE_POT_CONTROL 0
#define PRINT_CALLBACK  0

#define PRINT(s, v) { Serial.print(F(s)); Serial.print(v); }

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 11

#define CLK_PIN   13  // or SCK
#define DATA_PIN  11  // or MOSI
#define CS_PIN    10  // or SS

// SPI hardware interface
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary pins
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Scrolling parameters
#if USE_POT_CONTROL
#define SPEED_IN  A5
#else
#define SCROLL_DELAY  75  // in milliseconds
int delay_multiply = 12; // in milliseconds
#define DEFAULT_delay_factor  5 //
int delay_factor = DEFAULT_delay_factor;
#endif // USE_POT_CONTROL

#define CHAR_SPACING  1 // pixels between characters

// Global message buffers shared by Serial and Scrolling functions
#define BUF_SIZE  75
uint8_t curMessage[BUF_SIZE] = {"   <    "};
uint8_t newMessage[BUF_SIZE];
bool newMessageAvailable = false;

uint16_t  scrollDelay;  // in milliseconds



void scrollDataSink(uint8_t dev, MD_MAX72XX::transformType_t t, uint8_t col)
// Callback function for data that is being scrolled off the display
{
#if PRINT_CALLBACK
  Serial.print("\n cb ");
  Serial.print(dev);
  Serial.print(' ');
  Serial.print(t);
  Serial.print(' ');
  Serial.println(col);
#endif
}

uint8_t scrollDataSource(uint8_t dev, MD_MAX72XX::transformType_t t)
// Callback function for data that is required for scrolling into the display
{
  static uint8_t* p = curMessage;
  static enum { NEW_MESSAGE, LOAD_CHAR, SHOW_CHAR, BETWEEN_CHAR } state = LOAD_CHAR;
  static uint8_t  curLen, showLen;
  static uint8_t  cBuf[15];
  uint8_t colData = 0;    // blank column is the default

#if IMMEDIATE_NEW
  if (newMessageAvailable)  // there is a new message waiting
  {
    state = NEW_MESSAGE;
    mx.clear(); // clear the display
  }
#endif

  // finite state machine to control what we do on the callback
  switch(state)
  {
    case NEW_MESSAGE:   // Load the new message
      memcpy(curMessage, newMessage, BUF_SIZE);	// copy it in
      newMessageAvailable = false;    // used it!
      p = curMessage;
      state = LOAD_CHAR;
      break;

    case LOAD_CHAR: // Load the next character from the font table
      showLen = mx.getChar(*p++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
      curLen = 0;
      state = SHOW_CHAR;

      // if we reached end of message, opportunity to load the next
      if (*p == '\0')
      {
        p = curMessage;     // reset the pointer to start of message
#if !IMMEDIATE_NEW
        if (newMessageAvailable)  // there is a new message waiting
        {
          state = NEW_MESSAGE;    // we will load it here
          break;
        }
#endif
      }
      // !! deliberately fall through to next state to start displaying

    case SHOW_CHAR: // display the next part of the character
      colData = cBuf[curLen++];
      if (curLen == showLen)
      {
        showLen = CHAR_SPACING;
        curLen = 0;
        state = BETWEEN_CHAR;
      }
      break;

    case BETWEEN_CHAR: // display inter-character spacing (blank columns)
      colData = 0;
      curLen++;
      if (curLen == showLen)
        state = LOAD_CHAR;
      break;

    default:
      state = LOAD_CHAR;
  }

  return(colData);
}

 void scrollText(void)
{
  static uint32_t	prevTime = 0;

  // Is it time to scroll the text?
  if (millis()-prevTime >= scrollDelay)
  {
    mx.transform(MD_MAX72XX::TSL);  // scroll along - the callback will load all the data
    prevTime = millis();      // starting point for next time
  }
}

uint16_t getScrollDelay(void)
{
#if USE_POT_CONTROL
  uint16_t  t;

  t = analogRead(SPEED_IN);
  t = map(t, 0, 1023, 25, 250);

  return(t);
#else
  return(SCROLL_DELAY);
#endif
}



////////
// PACman
///////
// --------------------
// Constant parameters
//
#define DEFAULT_ANIMATION_DELAY 75  // milliseconds
#define MAX_FRAMES      4   // number of animation frames

// ========== General Variables ===========
//
const uint8_t pacman[MAX_FRAMES][18] =  // ghost pursued by a pacman
{
  { 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0x7e, 0xff, 0xe7, 0xc3, 0x81, 0x00 },
  { 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0xff, 0xff, 0xe7, 0xe7, 0x42, 0x00 },
  { 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xe7, 0x66, 0x24 },
  { 0xfe, 0x7b, 0xf3, 0x7f, 0xf3, 0x7b, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c },
};
const uint8_t DATA_WIDTH = (sizeof(pacman[0])/sizeof(pacman[0][0]));

uint32_t prevTimeAnim = 0;  // remember the millis() value in animations
int16_t idx;                // display index (column)
uint8_t frame;              // current animation frame
uint8_t deltaFrame;         // the animation frame offset for the next frame

// ========== Control routines ===========
//
void resetMatrix(void)
{
  mx.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY/2);
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  mx.clear();
}
void animate_start(int ani_delay = DEFAULT_ANIMATION_DELAY,boolean clear_first = true)
{
  Serial.print(" animate_start: "); 
  Serial.println(ani_delay); 

  if(clear_first){
    Serial.println("clear_first"); 
    resetMatrix();  
  }
  prevTimeAnim = millis();
  boolean bInit = true;  // initialise the animation
  idx = -DATA_WIDTH;
  
  while(bInit){
//    Serial.print("Animate: ");Serial.println(idx);
//    Serial.print("idx max: ");Serial.println(mx.getColumnCount()+DATA_WIDTH);
    // Is it time to animate?
//    if (millis()-prevTimeAnim < DEFAULT_ANIMATION_DELAY)
//      return;

//    if(millis()-prevTimeAnim > DEFAULT_ANIMATION_DELAY){
    if(millis()-prevTimeAnim > ani_delay){
      prevTimeAnim = millis();      // starting point for next time
    
      mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
      
//      Serial.print("Animate1: ");Serial.println(idx);
//      mx.clear();
    
      // Initialize
      if (idx == -DATA_WIDTH)
      {
//        Serial.print("Animate2: ");Serial.println(idx);
//        mx.clear();
  //      idx = -DATA_WIDTH;
        frame = 0;
        deltaFrame = 1;
  //      bInit = false;

        if(clear_first){
          // Lay out the dots
          for (uint8_t i=0; i<MAX_DEVICES; i++)
          {
            mx.setPoint(3, (i*COL_SIZE) + 3, true);
            mx.setPoint(4, (i*COL_SIZE) + 3, true);
            mx.setPoint(3, (i*COL_SIZE) + 4, true);
            mx.setPoint(4, (i*COL_SIZE) + 4, true);
          }
        }
      }
    
      // now run the animation
//      PRINT("\nINV I:", idx);
//      PRINT(" frame ", frame);
    
      // clear old graphic
      for (uint8_t i=0; i<DATA_WIDTH; i++)
        mx.setColumn(idx-DATA_WIDTH+i, 0);
      // move reference column and draw new graphic
      idx++;
      for (uint8_t i=0; i<DATA_WIDTH; i++)
        mx.setColumn(idx-DATA_WIDTH+i, pacman[frame][i]);
    
      // advance the animation frame
      frame += deltaFrame;
      if (frame == 0 || frame == MAX_FRAMES-1)
        deltaFrame = -deltaFrame;
       
      mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
    }
    
    // check if we are completed and set initialise for next time around
//    bInit = (idx == mx.getColumnCount()+DATA_WIDTH);
//    Serial.print("bInit2.5: ");Serial.println(bInit);
//    Serial.print("Animate2.5: ");Serial.println(idx);
//    Serial.print("idx max2.5: ");Serial.println(mx.getColumnCount()+DATA_WIDTH);    
//    bInit = idx+DATA_WIDTH < (mx.getColumnCount()); // 
    bInit = idx+DATA_WIDTH < 50; // 
//    Serial.print("Animate3: ");Serial.println(idx);
//    Serial.print("bInit3: ");Serial.println(bInit);    
  }
  return;
}


///////
// END part of PACMAN
//////

void readSerial(void)
{
  static uint8_t  putIndex = 0;
  boolean found_delay = false;

  while (Serial.available() &&  !found_delay)
  {
    newMessage[putIndex] = (char)Serial.read();
    if ((newMessage[putIndex] == '\n') || (putIndex >= BUF_SIZE-3)) // end of message character or full buffer
    {
      // put in a message separator and end the string
      newMessage[putIndex++] = ' ';
      newMessage[putIndex] = '\0';
      // restart the index for next filling spree and flag we have a message waiting
      putIndex = 0;
      newMessageAvailable = true;
//      Serial.print("putIndex1: ");Serial.println(putIndex);
    }
    else if (newMessage[putIndex] != '\r')
    {
      if(newMessage[putIndex] == ',')
       {
//        Serial.print("found delay: ");
//        Serial.print(putIndex); 
//        Serial.print(" > "); 
//        Serial.print((char)newMessage[putIndex+1]); 
//        delay_factor = atoi((char)newMessage[putIndex+1]);
        delay(20);
        char char_delay_factor = (char)Serial.read(); // Convert to char
        int int_delay_factor = char_delay_factor -48;  // Convert char 1 to number 1 ('0' is 48
        Serial.print(" Delay1: "); 
        Serial.println(char_delay_factor); 
        Serial.println(int_delay_factor); 
        if(int_delay_factor == 64){ // if delay with "p"
          Serial.print("found PACMAN>");
          delay(20);
          char char_delay_factor_pac = (char)Serial.read(); // Convert char 1 to number 1 ('0' is 48
          int int_delay_factor_pac = char_delay_factor_pac -48;  // Convert char 1 to number 1 ('0' is 48
          if(int_delay_factor_pac != -38){
            animate_start(int_delay_factor_pac*5, false); // if with delay for pacman
          }else{
            animate_start(DEFAULT_delay_factor*5, true); // without delay for pacman
          }
          Serial.println(char_delay_factor_pac);           
          Serial.println(int_delay_factor_pac);
          
        }else{
          delay_factor = int_delay_factor; // update delay_factor if no PACMAN
        }
        
        
        // put in a message separator and end the string
        newMessage[putIndex++] = ' ';
        newMessage[putIndex] = '\0';
        // restart the index for next filling spree and flag we have a message waiting
        newMessageAvailable = true;
        found_delay = true;
//        Serial.print("putIndex2: ");Serial.println(putIndex);
        
      }else{
          delay_factor = DEFAULT_delay_factor;
        }
        // Just save the next char in next location
        putIndex++;
//        Serial.print("putIndex3: ");Serial.println(putIndex);
    }
//  Serial.print("putIndex4: ");Serial.println(putIndex);
//  Serial.print("Delay2: "); 
//  Serial.println(delay_factor); 
  }
}






void setup()
{
  mx.begin();
  mx.setShiftDataInCallback(scrollDataSource);
  mx.setShiftDataOutCallback(scrollDataSink);

#if USE_POT_CONTROL
  pinMode(SPEED_IN, INPUT);
#else
  scrollDelay = SCROLL_DELAY;
#endif

  newMessage[0] = '\0';

  Serial.begin(57600);
  Serial.print("\n[MD_MAX72XX Message Display]\nType a message for the scrolling display\nEnd message line with a newline");
  
  animate_start(20);
}

void loop()
{
  //scrollDelay = getScrollDelay();
  scrollDelay = delay_factor*delay_multiply;
  readSerial();
  scrollText();
}
