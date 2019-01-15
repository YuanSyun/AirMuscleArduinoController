/*
    Name: CKD_and_Solenoid_Valve.ino
    Date: 12/25/2018
    Coder: Yuansyun Ye (yuansyuntw@gmail.com)
*/



const bool DEBUG = false;



//CKD Valve (比例閥)
#define CKD_PIN 9  //analogWrite
#define INPUT_COMMAND_SIZE 256
char input_string[INPUT_COMMAND_SIZE];
int level_percentage = 0;
float level_PWM = 0.0f;
const float percentage_to_PWM = 2.55; //假設0V = 0%、5V = 100%，因為PWM是0~254，故1% = 255/100 PWM        



//Solenoid Valve (電磁閥)
#define LSV_PIN1 2
#define LSV_PIN2 3
#define LSV_PIN3 4
#define LSV_PIN4 5
bool flag_left_sv1 = false;
bool flag_left_sv2 = false;
bool flag_left_sv3 = false;
bool flag_left_sv4 = false;

unsigned long delay_time = 50;
unsigned long last_loop_time;



void setup()                         
{
  /* Waiting the arduino setup... */
  delay(1000);
  Serial.begin(14400);
  Serial.print("Start up...\n");
  
  //CKD valve
  pinMode(CKD_PIN, OUTPUT);
  analogWrite(CKD_PIN, 0);

  //Solenoid Valve
  pinMode(LSV_PIN1, OUTPUT);
  pinMode(LSV_PIN2, OUTPUT);
  pinMode(LSV_PIN3, OUTPUT);
  pinMode(LSV_PIN4, OUTPUT);
  digitalWrite(LSV_PIN1, 0);
  digitalWrite(LSV_PIN2, 0);
  digitalWrite(LSV_PIN3, 0);
  digitalWrite(LSV_PIN4, 0);
}



void loop() 
{ 
  if(millis()- last_loop_time < delay_time)
  {
    return;
  }

    
  // 假設0V = 0%、 5V = 100%，又因為PWM是0~254，故1% = 255/100 PWM，
  // 也就是輸入x，則輸出值為100/255*x
  // 經廠商測試，閥與實驗室現有空壓機搭配，可接受的電壓值的量大約是2.5V
  
  //input：(閥的開關比例, 閥開/關)

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
        level_percentage = atoi(command);
        level_PWM = level_percentage * percentage_to_PWM;
      }else if(command_index == 1)
      {
         flag_left_sv1 = atoi(command);
      }else if(command_index == 2)
      {
        flag_left_sv2 = atoi(command);
      }else if(command_index == 3)
      {
        flag_left_sv3 = atoi(command);
      }else if(command_index == 4)
      {
        flag_left_sv4 = atoi(command);
      }
      
      command_index += 1;
      command = strtok(NULL, " ");
    }

    if(command_index == 5){
      analogWrite(CKD_PIN, level_PWM);
      digitalWrite(LSV_PIN1, flag_left_sv1);
      digitalWrite(LSV_PIN2, flag_left_sv2);
      digitalWrite(LSV_PIN3, flag_left_sv3);
      digitalWrite(LSV_PIN4, flag_left_sv4);

      if(DEBUG){
        char debug_info[256];
        char temp_pwm[10];
        dtostrf(level_PWM, 3,1, temp_pwm);
        sprintf(debug_info, "air pressure: (%d/100, %s/255)\nleft solenoiad valve state: %d %d %d %d\n", level_percentage, temp_pwm, flag_left_sv1, flag_left_sv2, flag_left_sv3, flag_left_sv4);
        Serial.print(debug_info);
      }
    }else{
        Serial.print("Wrong receive format(INT BOOL BOOL BOOL BOOL)\n");
    }

    /* Clear the input string */
    memset(input_string, 0, sizeof(input_string));
  }

  last_loop_time = millis();
}//end loop


