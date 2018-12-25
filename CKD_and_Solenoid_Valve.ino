/*
    Name: CKD_and_Solenoid_Valve.ino
    Date: 12/25/2018
    Coder: Yuansyun Ye (yuansyuntw@gmail.com)
*/


//CKD valve
#define Valve_Control_PIN 9  //analogWrite
String inputString = "";
bool stringComplete = false;
int level_percentage = 0;
float level_PWM = 0.0f;
const float percentage_to_PWM = 2.55; //假設0V = 0%、5V = 100%，因為PWM是0~254，故1% = 255/100 PWM

//Solenoid valve
#define solenoidPin 4    //digitalWrite
int solenoidSwitch = 0; //預設為關

void setup()                         
{
  Serial.begin(9600);
  
  //CKD valve (比例閥)
  pinMode(Valve_Control_PIN, OUTPUT);
  analogWrite(Valve_Control_PIN, 0);

  //Solenoid valve (電磁閥)
  pinMode(solenoidPin, OUTPUT);           //Sets the pin as an output
  digitalWrite(solenoidPin, LOW);
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
    if (c == '\n') 
    {
      stringComplete = true;   
    }

    if (stringComplete) 
    {
      for (int i = 0; i < inputString.length(); i++) 
      {
        if (inputString.substring(i, i + 1) == " ")
        {
          level_percentage = inputString.substring(0, i).toInt();
          solenoidSwitch = inputString.substring(i + 1).toInt();
          break;
        }
      }

      level_PWM = percentage_to_PWM * level_percentage;
      Serial.print("level(%): ");
      Serial.println(level_percentage);
      Serial.print("level(255): ");
      Serial.println(level_PWM, 2);
      analogWrite(Valve_Control_PIN, level_PWM);
      stringComplete = false;
      inputString = "";
      delay(10);


      if (solenoidSwitch == 0)
      {
        digitalWrite(solenoidPin, LOW);    //Switch Solenoid OFF
        Serial.println("Switch Solenoid OFF");
      }  
      if (solenoidSwitch == 1)
      {
        digitalWrite(solenoidPin, HIGH);    //Switch Solenoid ON
        Serial.println("Switch Solenoid ON");
      }
    }
  }
}


