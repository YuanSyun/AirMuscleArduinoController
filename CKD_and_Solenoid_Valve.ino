/*
    Name: CKD_and_Solenoid_Valve.ino
    Date: 12/25/2018
    Coder: Yuansyun Ye (yuansyuntw@gmail.com)
*/



const bool DEBUG = true;



//CKD valve
#define Valve_Control_PIN 9  //analogWrite
String inputString = "";
bool stringComplete = false;
int level_percentage = 0;
float level_PWM = 0.0f;
const float percentage_to_PWM = 2.55; //假設0V = 0%、5V = 100%，因為PWM是0~254，故1% = 255/100 PWM



void setup()                         
{
  Serial.begin(9600);
  
  //CKD valve (比例閥)
  pinMode(Valve_Control_PIN, OUTPUT);
  analogWrite(Valve_Control_PIN, 0);
}



void loop() 
{ 
  // 假設0V = 0%、 5V = 100%，又因為PWM是0~254，故1% = 255/100 PWM，
  // 也就是輸入x，則輸出值為100/255*x
  // 經廠商測試，閥與實驗室現有空壓機搭配，可接受的電壓值的量大約是2.5V
  
  //input：(閥的開關比例, 閥開/關)
  
  while(Serial.available())
  {
    char c = Serial.read();
    inputString += c;

    /* When reading the command end */
    if (c == '\n') 
    {
      level_percentage = inputString.toInt();

      level_PWM = percentage_to_PWM * level_percentage;
      if(DEBUG){
          Serial.print("level(%): ");
          Serial.println(level_percentage);
          Serial.print("level(255): ");
          Serial.println(level_PWM, 2);
      }
      analogWrite(Valve_Control_PIN, level_PWM);
      
      inputString = "";
      delay(10);
    }//end if
  }//end while
  
}//end loop


