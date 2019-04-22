/*
    Name: CKD_and_Solenoid_Valve.ino
    Date: 12/25/2018
    Coder: Yuansyun Ye (yuansyuntw@gmail.com)

    Command Example:
        Release Air: 0 1 1 1 1 0 1 1 1 1
        Close the Valve: 0 0 0 0 0 0 0 0 0 0
        Test Air: 5 0 0 1 0 5 0 0 1 0
        Keep Air: 10 0 0 0 0 10 0 0 0 0
*/

#include <SparkFun_ADXL345.h>

/* Debug setting */
const bool DEBUG = false;

/* CKD Valve (比例閥)*/    
#define LCKD_PIN 10         // analogWrite
#define RCKD_PIN 11         // analogWrite
#define INPUT_COMMAND_SIZE 256
int LCKD_PRESSURE = A2;     // reading the valve pressure
int RCKD_PRESSURE = A3;     // reading the valve pressure
int left_percentage = 0;
int right_percentage = 0;
int left_pressure = 0;
int right_pressure = 0;
float left_PWM = 0.0f;
float right_PWM = 0.0f;
const float percentage_to_PWM = 2.55; //假設0V = 0%、5V = 100%，因為PWM是0~254，故1% = 255/100 PWM        

/* Solenoid Valve (電磁閥) */
#define LSV_PIN1 2
#define LSV_PIN2 3
#define LSV_PIN3 4
#define LSV_PIN4 5
#define RSV_PIN1 6
#define RSV_PIN2 7
#define RSV_PIN3 8
#define RSV_PIN4 9

/* The 16 relay model is low level triger */
bool flag_left_sv1 = false;
bool flag_left_sv2 = false;
bool flag_left_sv3 = false;
bool flag_left_sv4 = false;
bool flag_right_sv1 = false;
bool flag_right_sv2 = false;
bool flag_right_sv3 = false;
bool flag_right_sv4 = false;

/* The loop setting */
unsigned long delay_time = 50;
unsigned long last_loop_time;

/* The serial command count totally is ten */
char input_string[INPUT_COMMAND_SIZE];
const int TOTAL_COMMAND_NUMBER = 10;

/* read the CKD valve timer*/
unsigned long reading_data_interval = 100;
unsigned long last_reading_data;

ADXL345 adxl = ADXL345();
int pumpState;



void setup()                         
{
  /* Waiting the arduino setup... */
  delay(500);
  Serial.begin(57600);
  //Serial.begin(9600);
  Serial.print("Start up...\n");
  
  //CKD valve
  pinMode(LCKD_PIN, OUTPUT);
  pinMode(RCKD_PIN, OUTPUT);
  pinMode(LCKD_PRESSURE, INPUT);
  pinMode(RCKD_PRESSURE, INPUT);
  analogWrite(LCKD_PIN, 0);
  analogWrite(RCKD_PIN, 0);
  

  //Solenoid Valve
  pinMode(LSV_PIN1, OUTPUT);
  pinMode(LSV_PIN2, OUTPUT);
  pinMode(LSV_PIN3, OUTPUT);
  pinMode(LSV_PIN4, OUTPUT);
  pinMode(RSV_PIN1, OUTPUT);
  pinMode(RSV_PIN2, OUTPUT);
  pinMode(RSV_PIN3, OUTPUT);
  pinMode(RSV_PIN4, OUTPUT);
  DigitalWrite(LSV_PIN1, !flag_left_sv1);   //The 16 relay model is low level triger
  DigitalWrite(LSV_PIN2, !flag_left_sv2);
  DigitalWrite(LSV_PIN3, !flag_left_sv3);
  DigitalWrite(LSV_PIN4, !flag_left_sv4);
  DigitalWrite(RSV_PIN1, !flag_right_sv1);
  DigitalWrite(RSV_PIN2, !flag_right_sv2);
  DigitalWrite(RSV_PIN3, !flag_right_sv3);
  DigitalWrite(RSV_PIN4, !flag_right_sv4);

  adxl.powerOn();
  adxl.setRangeSetting(16);
  adxl.setActivityXYZ(1, 0, 0);
  adxl.setActivityThreshold(75);
  adxl.setInactivityXYZ(1, 0, 0);
  adxl.setInactivityThreshold(75);
  adxl.setTimeInactivity(10);
  adxl.setFreeFallThreshold(7);
  adxl.setFreeFallDuration(30);
  adxl.InactivityINT(1);
  adxl.ActivityINT(1);
  adxl.FreeFallINT(1);
  
}



void loop() 
{ 

  /* Reading the CKD valve pressure value */
  if(millis() - last_reading_data > reading_data_interval)
  {
    left_pressure = analogRead(LCKD_PRESSURE);
    right_pressure = analogRead(RCKD_PRESSURE);
    pumpState = GetADXLState();

    char pressure[50];
    sprintf(pressure, "%d %d %d\n", left_pressure, right_pressure, pumpState);
    Serial.print(pressure);
    Serial.flush();
    last_reading_data = millis();
  }
  
  if(millis() - last_loop_time < delay_time)
  {
    return;
  }

  /* Reading the serial command */
  String input_command = "";
  while(Serial.available())
  {
    char c = Serial.read();
    input_command += c;
    /* When reading the command end */
    if (c == '\n'){
        if(DEBUG)
        {
          String debug = "\n[Receive]" + input_command;
          Serial.print(debug);
        }
      break;
    }
  }

  // 假設0V = 0%、 5V = 100%，又因為PWM是0~254，故1% = 255/100 PWM，
  // 也就是輸入x，則輸出值為100/255*x
  // 經廠商測試，閥與實驗室現有空壓機搭配，可接受的電壓值的量大約是2.5V
  //input：(閥的開關比例, 閥開/關)

  /* Perform the command */
  if(input_command != ""){

    /* Srting convert to char array */
    input_command.toCharArray(input_string, INPUT_COMMAND_SIZE);

    /* Slice the input string with space */
    char* command = strtok(input_string, " ");
    int command_index = 0;
    while(command != 0)
    {
      if(command_index == 0)
      {
        left_percentage = atoi(command);
        left_PWM = left_percentage * percentage_to_PWM;
      }else if(command_index == 1)
      {
         //The 16 relay model is low level triger
         flag_left_sv1 = !atoi(command);
      }else if(command_index == 2)
      {
        flag_left_sv2 = !atoi(command);
      }else if(command_index == 3)
      {
        flag_left_sv3 = !atoi(command);
      }else if(command_index == 4)
      {
        flag_left_sv4 = !atoi(command);
      }else if(command_index == 5)
      {
        right_percentage = atoi(command);
        right_PWM = right_percentage * percentage_to_PWM;
      }else if(command_index == 6)
      {
        flag_right_sv1 = !atoi(command);
      }else if(command_index == 7)
      {
        flag_right_sv2 = !atoi(command);
      }else if(command_index == 8)
      {
        flag_right_sv3 = !atoi(command);
      }else if(command_index == 9)
      {
        flag_right_sv4 = !atoi(command);
      }
      
      command_index += 1;
      command = strtok(NULL, " ");
    }

    if(command_index == TOTAL_COMMAND_NUMBER){
      DigitalWrite(LSV_PIN1, flag_left_sv1);
      DigitalWrite(LSV_PIN2, flag_left_sv2);
      DigitalWrite(LSV_PIN3, flag_left_sv3);
      DigitalWrite(LSV_PIN4, flag_left_sv4);
      DigitalWrite(RSV_PIN1, flag_right_sv1);
      DigitalWrite(RSV_PIN2, flag_right_sv2);
      DigitalWrite(RSV_PIN3, flag_right_sv3);
      DigitalWrite(RSV_PIN4, flag_right_sv4);
      analogWrite(LCKD_PIN, left_PWM);
      analogWrite(RCKD_PIN, right_PWM);

      if(DEBUG){
        char debug_info[256];
        char string_left_pwm[10];
        char string_right_pwm[10];
        dtostrf(left_PWM, 3, 1, string_left_pwm);
        dtostrf(right_PWM, 3, 1, string_right_pwm);
        sprintf(debug_info, "[left] air pressure: (%d/100, %s/255), solenoiad valve state: %d %d %d %d\n[right] air pressure:(%d/100, %s/255), solenoiad valve state: %d %d %d %d\n",
            left_percentage, string_left_pwm, flag_left_sv1, flag_left_sv2, flag_left_sv3, flag_left_sv4,
            right_percentage, string_right_pwm, flag_right_sv1, flag_right_sv2, flag_right_sv3, flag_right_sv4);
        Serial.print(debug_info);
      }
    }else{
        Serial.print("[Wrong] correct format(INT BOOL BOOL BOOL BOOL INT BOOL BOOL BOOL BOOL)\n");
    }

    /* Clear the input string */
    //memset(input_string, 0, sizeof(input_string));
  }

  last_loop_time = millis();
}//end loop

/* This is for Uno board, in order to speed up the digital write*/
void DigitalWrite(int _pin, bool _flag)
{
    if(_pin == 2){
        /* PD2 */
        if(_flag){
            PORTD |= B00000100;
        }else{
            PORTD &= B11111011;
        }
    }else if(_pin == 3){
        /* PD3 */
        if(_flag){
            PORTD |= B00001000;
        }else{
            PORTD &= B11110111;
        }
    }else if(_pin == 4){
        /* PD4 */
        if(_flag){
            PORTD |= B00010000;
        }else{
            PORTD &= B11101111;
        }
    }else if(_pin == 5){
        /* PD5*/
        if(_flag){
            PORTD |= B00100000;
        }else{
            PORTD &= B11011111;
        }
    }else if(_pin == 6){
        /* PD6 */
        if(_flag){
            PORTD |= B01000000;
        }else{
            PORTD &= B10111111;
        }
    }else if(_pin == 7){
        /* PD7 */
        if(_flag){
            PORTD |= B10000000;
        }else{
            PORTD &= B01111111;
        }
    }else if(_pin == 8){
        /* PB0 */
        if(_flag){
            PORTB |= B00000001;
        }else{
            PORTB &= B11111110;
        }
    }else if(_pin == 9){
        if(_flag){
            PORTB |= B00000010;
        }else{
            PORTB &= B11111101;
        }
    }
}

int GetADXLState()
{
    int state = 0;
    
  // getInterruptSource clears all triggered actions after returning value
  // Do not call again until you need to recheck for triggered actions
  byte interrupts = adxl.getInterruptSource();
  
  // Activity
  if(adxl.triggered(interrupts, ADXL345_ACTIVITY)){
    state = 1;
  }

  return state;
}
