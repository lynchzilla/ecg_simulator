// ***************************************************************************************************
//                                    ECG SIMULATOR
//
//  Purpose: simulate the normal sinus rhythm ECG signal (3-leads RA,LA,RL)
//
//  Background:
// 
//  In normal electrocardiography (ECG or EKG if you're German), three leads make up the
//  Einthoven's triangle. Two leads are taped to the right and left side of the chest above
//  the heart (RA = right arm, LA = left arm) and one lead is taped to the lower chest, typically
//  on the right hip (RL = right leg). 
//                                           
//  It's important to know that these ECG signals are millivolts in amplitude. This can be achieved by          
//  feeding the D/A converter through a voltage divider to get to the millivolt levels.
//
//  
//  The ECG signal:
//
//  I found a suitable ECG waveform from the internet. Here is how I converted a picture from my
//  monitor screen to a C language array of A/D values, each spaced 1.00 msec apart.
//                                           
//       A. Screen shot of waveform using the free screen capture program MWSNAP          
//                http://www.mirekw.com/winfreeware/mwsnap.html 
//
//       B. Digitize the jpeg waveform using the free digitizing program ENGAUGE
//                http://digitizer.sourceforge.net/
//
//            
//       C: I wrote a Python program to convert the rather irregular samples from ENGAUGE
//          to an array of values spaced 1.0 milliseconds apart using linear interpolation.                                 
//          Then I created a text file where these data points were part of a C language array
//          construct; that is, the data points are C initializers. 
//
//       D: Cut-and-paste from the text file the C data array with initializers into the 
//          Arduino sketch below.
//  
//
//  Arduino Resources:   
//
//    Digital Output # 9  - chip select the 7-segment display SPI port (low to select)
//    Digital Output # 10 - chip select for D/A converter  (low to select)
//    Digital Output # 11 - SDI data to the D/A converter  (SPI interface)
//    Digital Output # 13 - SCK clock to the D/A converter (SPI interface)
//  
//    Analog Input   # 0  - center wiper pin of 5k ohm pot (heart rate adjust)
//
//    I followed the Timer2 setup as outlined by Sebastian Wallin 
//    http://popdevelop.com/2010/04/mastering-timer-interrupts-on-the-arduino/
//
//    I set up the SPI interface according to the excellent instructions of Australian John Boxall,
//    whose wonderful website has many excellent Arduino tutorials: 
//    http://tronixstuff.wordpress.com/
//
//  Programmer:  James P Lynch
//               lynch007@gmail.com
//  
// ***************************************************************************************************

#include "SPI.h"          // supports the SPI interface to the D/A converter and 7-segment display
#include <Wire.h>         // need the Wire library

// various constants used by the waveform generator
#define INIT      0
#define IDLE      1
#define QRS       2
#define FOUR      4
#define THREE     3
#define TWO       2
#define ONE       1



// *******************************************************************************
//   y_data[543] - digitized ecg waveform, sampled at 1.0 msec
//
//   Waveform is scaled for a 12-bit D/A converter (0 .. 4096)
//
//   A 60 beat/min ECG would require this waveform (543 samples) plus 457 samples 
//   of the first y_data[0] value of 936.
//
// *******************************************************************************
const short  y_data[] = {
939, 940, 941, 942, 944, 945, 946, 947, 951, 956, 
962, 967, 973, 978, 983, 989, 994, 1000, 1005, 1015, 
1024, 1034, 1043, 1053, 1062, 1075, 1087, 1100, 1112, 1121, 
1126, 1131, 1136, 1141, 1146, 1151, 1156, 1164, 1172, 1179, 
1187, 1194, 1202, 1209, 1216, 1222, 1229, 1235, 1241, 1248, 
1254, 1260, 1264, 1268, 1271, 1275, 1279, 1283, 1287, 1286, 
1284, 1281, 1279, 1276, 1274, 1271, 1268, 1266, 1263, 1261, 
1258, 1256, 1253, 1251, 1246, 1242, 1237, 1232, 1227, 1222, 
1218, 1215, 1211, 1207, 1203, 1199, 1195, 1191, 1184, 1178, 
1171, 1165, 1159, 1152, 1146, 1141, 1136, 1130, 1125, 1120, 
1115, 1110, 1103, 1096, 1088, 1080, 1073, 1065, 1057, 1049, 
1040, 1030, 1021, 1012, 1004, 995, 987, 982, 978, 974, 
970, 966, 963, 959, 955, 952, 949, 945, 942, 939, 
938, 939, 940, 941, 943, 944, 945, 946, 946, 946, 
946, 946, 946, 946, 946, 947, 950, 952, 954, 956, 
958, 960, 962, 964, 965, 965, 965, 965, 965, 965, 
963, 960, 957, 954, 951, 947, 944, 941, 938, 932, 
926, 920, 913, 907, 901, 894, 885, 865, 820, 733, 
606, 555, 507, 632, 697, 752, 807, 896, 977, 1023, 
1069, 1127, 1237, 1347, 1457, 2085, 2246, 2474, 2549, 2595, 
2641, 2695, 3083, 3135, 3187, 3217, 3315, 3403, 3492, 3581, 
3804, 3847, 3890, 3798, 3443, 3453, 3297, 3053, 2819, 2810, 
2225, 2258, 1892, 1734, 1625, 998, 903, 355, 376, 203, 
30, 33, 61, 90, 119, 160, 238, 275, 292, 309, 
325, 343, 371, 399, 429, 484, 542, 602, 652, 703, 
758, 802, 838, 856, 875, 895, 917, 938, 967, 1016, 
1035, 1041, 1047, 1054, 1060, 1066, 1066, 1064, 1061, 1058, 
1056, 1053, 1051, 1048, 1046, 1043, 1041, 1038, 1035, 1033, 
1030, 1028, 1025, 1022, 1019, 1017, 1014, 1011, 1008, 1006, 
1003, 1001, 999, 998, 996, 994, 993, 991, 990, 988, 
986, 985, 983, 981, 978, 976, 973, 971, 968, 966, 
963, 963, 963, 963, 963, 963, 963, 963, 963, 963, 
963, 963, 963, 963, 963, 963, 963, 963, 963, 963, 
964, 965, 966, 967, 968, 969, 970, 971, 972, 974, 
976, 978, 980, 983, 985, 987, 989, 991, 993, 995, 
997, 999, 1002, 1006, 1011, 1015, 1019, 1023, 1028, 1032, 
1036, 1040, 1045, 1050, 1055, 1059, 1064, 1069, 1076, 1082, 
1088, 1095, 1101, 1107, 1114, 1120, 1126, 1132, 1141, 1149, 
1158, 1166, 1173, 1178, 1183, 1188, 1193, 1198, 1203, 1208, 
1214, 1221, 1227, 1233, 1240, 1246, 1250, 1254, 1259, 1263, 
1269, 1278, 1286, 1294, 1303, 1309, 1315, 1322, 1328, 1334, 
1341, 1343, 1345, 1347, 1349, 1351, 1353, 1355, 1357, 1359, 
1359, 1359, 1359, 1359, 1358, 1356, 1354, 1352, 1350, 1347, 
1345, 1343, 1341, 1339, 1336, 1334, 1332, 1329, 1327, 1324, 
1322, 1320, 1317, 1315, 1312, 1307, 1301, 1294, 1288, 1281, 
1275, 1270, 1265, 1260, 1256, 1251, 1246, 1240, 1233, 1227, 
1221, 1214, 1208, 1201, 1194, 1186, 1178, 1170, 1162, 1154, 
1148, 1144, 1140, 1136, 1131, 1127, 1123, 1118, 1114, 1107, 
1099, 1090, 1082, 1074, 1069, 1064, 1058, 1053, 1048, 1043, 
1038, 1034, 1029, 1025, 1021, 1017, 1013, 1009, 1005, 1001, 
997, 994, 990, 991, 992, 994, 996, 997, 999, 998, 
997, 996, 995, 994, 993, 991, 990, 989, 989, 989, 
989, 989, 989, 989, 988, 986, 984, 983, 981, 980, 
982, 984, 986, 988, 990, 993, 995, 997, 999, 1002, 
1005, 1008, 1012};

 
// global variables used by the program
unsigned int  NumSamples = sizeof(y_data) / 2;            // number of elements in y_data[] above
unsigned int  QRSCount = 0;                               // running QRS period msec count
unsigned int  IdleCount = 0;                              // running Idle period msec count
unsigned long IdlePeriod = 0;                             // idle period is adjusted by pot to set heart rate
unsigned int  State = INIT;                               // states are INIT, QRS, and IDLE
unsigned int  DisplayCount = 0;                           // counts 50 msec to update the 7-segment display
unsigned int  tcnt2;                                      // Timer2 reload value, globally available
float         BeatsPerMinute;                             // floating point representation of the heart rate
unsigned int  Bpm;                                        // integer version of heart rate (times 10)
unsigned int  BpmLow;                                     // lowest heart rate allowed (x10)
unsigned int  BpmHigh;                                    // highest heart rate allowed (x10)
int           Value;                                      // place holder for analog input 0
unsigned long BpmValues[32] = {0, 0, 0, 0, 0, 0, 0, 0,    // holds 32 last analog pot readings
                               0, 0, 0, 0, 0, 0, 0, 0,    // for use in filtering out display jitter
                               0, 0, 0, 0, 0, 0, 0, 0,    // for use in filtering out display jitter
                               0, 0, 0, 0, 0, 0, 0, 0};   // for use in filtering out display jitter
unsigned long BpmAverage = 0;                             // used in a simple averaging filter
unsigned char Index = 0;                                  // used in a simple averaging filter  
unsigned int  DisplayValue = 0;                           // filtered Beats Per Minute sent to display


void setup()   { 
  
  // Configure the output ports (1 msec intrerrupt indicator and D/A SPI support)
  pinMode(9, OUTPUT);                     // 7-segment display chip select   (low to select chip) 
  pinMode(10, OUTPUT);                    // D/A converter chip select       (low to select chip) 
  pinMode(11, OUTPUT);                    // SDI data 
  pinMode(13, OUTPUT);                    // SCK clock

  // initial state of SPI interface
  SPI.begin();                            // wake up the SPI bus.
  SPI.setDataMode(0);                     // mode: CPHA=0, data captured on clock's rising edge (low→high)
  SPI.setClockDivider(SPI_CLOCK_DIV64);   // system clock / 64
  SPI.setBitOrder(MSBFIRST);              // bit 7 clocks out first
  
  // establish the heart rate range allowed
  // BpmLow  = 300 (30 bpm x 10)
  // BpmHigh = (60.0 / (NumSamples * 0.001)) * 10  = (60.0 / .543) * 10 = 1104  (110.49 x 10)
  BpmLow = 300;
  BpmHigh = (60.0 / ((float)NumSamples * 0.001)) * 10;

  // First disable the timer overflow interrupt while we're configuring
  TIMSK2 &= ~(1<<TOIE2);

  // Configure timer2 in normal mode (pure counting, no PWM etc.)
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
  TCCR2B &= ~(1<<WGM22);

  // Select clock source: internal I/O clock   
  ASSR &= ~(1<<AS2);

  // Disable Compare Match A interrupt enable (only want overflow)   
  TIMSK2 &= ~(1<<OCIE2A);

  // Now configure the prescaler to CPU clock divided by 128   
  TCCR2B |= (1<<CS22)  | (1<<CS20); // Set bits
  TCCR2B &= ~(1<<CS21);             // Clear bit

  // We need to calculate a proper value to load the timer counter.
  // The following loads the value 131 into the Timer 2 counter register
  // The math behind this is:
  // (CPU frequency) / (prescaler value) = 125000 Hz = 8us.
  // (desired period) / 8us = 125.
  // MAX(uint8) + 1 - 125 = 131;
  // 
  // Save value globally for later reload in ISR  /
  tcnt2 = 131; 

  // Finally load end enable the timer   
  TCNT2 = tcnt2;
  TIMSK2 |= (1<<TOIE2);
}


void loop() {

  // read from the heart rate pot (Analog Input 0)  
  Value = analogRead(0);
 
  // map the Analog Input 0 range (0 .. 1023) to the Bpm range (300 .. 1104)
  Bpm = map(Value, 0, 1023, BpmLow, BpmHigh);

  // To lessen the jitter or bounce in the display's least significant digit,
  // a moving average filter (32 values) will smooth it out.
  BpmValues[Index++] = Bpm;                       // add latest sample to eight element array
  if (Index == 32) {                              // handle wrap-around
    Index = 0;
  }
  BpmAverage = 0;
  for (int  i = 0;  i < 32; i++) {                // summation of all values in the array
    BpmAverage += BpmValues[i];
  }
  BpmAverage >>= 5;                               // Divide by 32 to get average
  
  // now update the 4-digit display - format: XXX.X
  // since update is a multi-byte transfer, disable interrupts until it's done
  noInterrupts();
  DisplayValue = BpmAverage;
  interrupts();  
  
  // given the pot value (beats per minute) read in, calculate the IdlePeriod (msec)
  // this value is used by the Timer2 1.0 msec interrupt service routine
  BeatsPerMinute = (float)Bpm / 10.0;
  noInterrupts();
  IdlePeriod = (unsigned int)((float)60000.0 / BeatsPerMinute) - (float)NumSamples;
  interrupts();
  
  delay(20);
}






// ********************************************************************************
//                Timer2 Interrupt Service Routine
//
// Interrupt Service Routine (ISR) for Timer2 overflow at 1.000 msec.
//
//
// The Timer2 interrupt function is used to send the 16-bit waveform point
// to the Microchip MCP4921 D/A converter using the SPI interface. 
//
// The Timer2 interrupt function is also used to send the current heart rate
// as read from the potentiometer every 50 Timer2 interrupts to the 7-segment display.
//
// The pot is read and the heart rate is calculated in the background loop. 
// By running both SPI peripherals at interrupt level, we "serialize" them and avoid 
// corruption by one SPI transmission being interrupted by the other.
//
// A state machime is implemented to accomplish this. It's states are:
//
//       INIT -    basically clears the counters and sets the state to QRS.
//
//       QRS  -    outputs the next ECG waveform data point every 1.0 msec
//                 there are 543 of these QRS complex data points.
//
//       IDLE -    variable period after the QRS part.
//                 D/A holds first ECG value (936) for all of the IDLE period.
//                 Idle period varies to allow adjustment of the basic heart rate; 
//                 a value of zero msec for the idle period gives 110.4 beats per min 
//                 while the maximum idle period of 457 msec gives 30.0 bpm.
//
//                 Note that the IDLE period is calculated in the main background 
//                 loop by reading a pot and converting its range to one suitable
//                 for the background period. The interrupt routine reads this 
//                 value to determine when to stop the IDLE period.
//
// The transmission of the next data point to the D/A converter via SPI takes
// about 63 microseconds (that includes two SPI byte transmissions).
//
// The transmission of the heart rate digits to the Sparkfun 7-segment display
// takes about 350 usec (it is only transmitted every 50 Timer2 interrupts)
//
// ********************************************************************************
ISR(TIMER2_OVF_vect) {
  
  // Reload the timer   
  TCNT2 = tcnt2;
  
  // state machine
  switch (State) {
    
    case INIT:

      // zero the QRS and IDLE counters
      QRSCount = 0;
      IdleCount = 0;
      DisplayCount = 0;
    
      // set next state to QRS  
      State = QRS;
    break;
  
    case QRS:

      // output the next sample in the QRS waveform to the D/A converter 
      DTOA_Send(y_data[QRSCount]);
      
      // advance sample counter and check for end
      QRSCount++;
      if (QRSCount >= NumSamples) {
        // start IDLE period and output first sample to DTOA
        QRSCount = 0;
        DTOA_Send(y_data[0]);
        State = IDLE;
      }
    break;
  
    case IDLE:

      // since D/A converter will hold the previous value written, all we have
      // to do is determine how long the IDLE period should be.
    
      // advance idle counter and check for end
      IdleCount++;
      
      // the IdlePeriod is calculated in the main loop (from a pot)
      if (IdleCount >= IdlePeriod) {
        IdleCount = 0;
        State = QRS;
      }  
    break;
  
    default:
    break;
  }
 
  // output to the 7-segment display every 50 msec
  DisplayCount++;
  if (DisplayCount >= 50) {
    DisplayCount = 0;
    Display7Seg_Send(DisplayValue);
  } 
}  




// ***************************************************************************************************
//                      void  DTOA_Send(unsigned short)
//
//  Purpose: send 12-bit D/A value to Microchip MCP4921 D/A converter ( 0 .. 4096 )
//
//
//  Input:    DtoAValue - 12-bit D/A value ( 0 .. 4096 )
//
//
//            The DtoAValue is prepended with the A/B, BUF, GA, and SHDN bits before transmission.
//
//                                           WRITE COMMAND
//            |--------|--------|--------|--------|--------------------------------------------------|
//            |  A/B   |   BUF  |  GA    | SHDN   | D11 D10 D09 D08 D07 D06 D05 D04 D03 D02 D01 D00  |
//            |        |        |        |        |                                                  |
//            |setting:|setting:|setting:|Setting:|            DtoAValue   (12 bits)                 |
//            |   0    |   0    |   1    |   1    |                                                  |
//            | DAC-A  |unbuffer|   1x   |power-on| ( 0 .. 4096  will output as 0 volts .. 5 volts ) |
//            |--------|--------|--------|--------|--------------------------------------------------|
//                15      14        13       12    11                                               0
//  To D/A    <======================================================================================
//
//            Note:  WriteCommand is clocked out with bit 15 first!
//
//
//  Returns:  nothing
//
//
//  I/O Resources:  Digital Pin 9  = chip select (low to select chip)
//                  Digital Pin 13 = SPI Clock
//                  Digital Pin 11 = SPI Data 
//
//  Note: by grounding the LDAC* pin in the hardware hook-up, the SPI data will be clocked into the 
//        D/A converter latches when the chip select rises at the end-of-transfer.
//
//        This routine takes 63 usec using an Adafruit Menta
// ***************************************************************************************************
void  DTOA_Send(unsigned short DtoAValue) {
  
  byte            Data = 0;
  // select the D/A chip (low)
  digitalWrite(10, 0);    // chip select low
  
  // send the high byte first 0011xxxx
  Data = highByte(DtoAValue);
  Data = 0b00001111 & Data;
  Data = 0b00110000 | Data;
  SPI.transfer(Data);
  
  // send the low byte next xxxxxxxx
  Data = lowByte(DtoAValue);
  SPI.transfer(Data);
 
  // all done, de-select the chip (this updates the D/A with the new value) 
  digitalWrite(10, 1);    // chip select high
}





// ***************************************************************************************************
//                      void  Display7Seg_Send(char *)
//
//  Purpose: send 4 digits to SparkFun Serial 7-Segment Display (requires 4 SPI writes)
//
//  Input:    value -  unsigned int version of BeatsPerMinute
//
//  Returns:  nothing
//
//  I/O Resources:  Digital Pin 10 = chip select (low to select chip)
//                  Digital Pin 13 = SPI Clock
//                  Digital Pin 11 = SPI Data 
//
//  Note:  this routine takes 350 usec using an Adafruit Menta
// ***************************************************************************************************
void  Display7Seg_Send(unsigned int  HeartRate) {
  uint8_t       digit1, digit2, digit3, digit4;
  unsigned int  value;
  
  // convert to four digits (set leading zeros to blanks; 0x78 is the blank character)
  value = HeartRate;
  digit1 = value / 1000;
  value -= digit1 * 1000;
  if (digit1 == 0) digit1 = 0x78;

  digit2 = value / 100;
  value -= digit2 * 100;
  if ((digit1 == 0x78) && (digit2 == 0)) digit2 = 0x78;

  digit3 = value / 10;
  value -= digit3 * 10;
  if ((digit1 == 0x78) && (digit2 == 0x78) && (digit3 == 0)) digit3 = 0x78;

  digit4 = value;

  digitalWrite(9, LOW);    // select the Sparkfun 7-seg display
  SPI.transfer(0x76);      // reset display
  SPI.transfer(0x7A);      // brightness command
  SPI.transfer(0x00);      // 0 = bright,  255 = dim
  SPI.transfer(digit1);    // Thousands Digit
  SPI.transfer(digit2);    // Hundreds Digit
  SPI.transfer(digit3);    // Tens Digit
  SPI.transfer(digit4);    // Ones Digit
  SPI.transfer(0x77);      // set decimal points command
  SPI.transfer(0x04);      // turn on dec pt between digits 3 and 4
  digitalWrite(9, HIGH);   // release Sparkfun 7-seg display
}




















