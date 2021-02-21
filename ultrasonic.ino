#include <LiquidCrystal.h>

int button1State = 0;
int button2State = 0;

int count = 0;
int a = 0;
int b = 0;
int old_state;
int new_state;

long measurement1;
long measurement2;
int psicount = 0;
int measurementcount = 0;
double knownO2 = 0;

bool firstmessage = true;
bool readultra = true;
bool ppm = true;

bool m2 = true;
bool calibrated = false;

double nitrogen = 0;
double oxygen = 0;

const double psi1 = 14.7; //psi of atm

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#define ROTARY1 A5
#define ROTARY2 A4
#define echoPin A3
#define pingPin A2
#define BUTTON1 13
#define BUTTON2 12



void setup() {


  pinMode(ROTARY1, INPUT);
  pinMode(ROTARY2, INPUT);
  pinMode(echoPin, INPUT);
  pinMode(pingPin, OUTPUT);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);


  //set up pull up resistors
  digitalWrite(ROTARY1, HIGH);
  digitalWrite(ROTARY2, HIGH);
  b = digitalRead(ROTARY1);
  a = digitalRead(ROTARY2);
  if (a != 0)
  {
    a = 1;
  }
  if (b != 0)
  {
    b = 1;
  }
  if (!b && !a)
    old_state = 0;
  else if (!b && a)
    old_state = 1;
  else if (b && !a)
    old_state = 2;
  else if (b && a)
    old_state = 3;

  new_state = old_state;
  Serial.begin(9600);

  lcd.begin(16, 2);
}

void checkRotary()
{
  b = digitalRead(ROTARY1);
  a = digitalRead(ROTARY2);
  if (a != 0)
  {
    a = 1;
  }
  if (b != 0)
  {
    b = 1;
  }
  if (old_state == 0) { // bits can go to either 1  or 2
    if (a && !b) //this 1
    {
      new_state = 1;
      count += 1;
    }
    else if (b && !a) //this is 2
    {
      new_state = 2;
      count -= 1;
    }
  }
  else if (old_state == 1) {
    if (!a && !b) //this is 0
    {
      new_state = 0;
      count -= 1;
    }
    else if (b && a) //this is 3
    {
      new_state = 3;
      count += 1;
    }
  }
  else if (old_state == 2) { //go to either 0 or 3
    if (!b && !a) //this is 0
    {
      new_state = 0;
      count += 1;
    }
    else if (a && b) //this is 3
    {
      new_state = 3;
      count -= 1;
    }
  }
  else {   // old_state = 3
    if (!b && a) //this is 1
    {
      new_state = 1;
      count -= 1;
    }
    else if (!a && b) //this is 2
    {
      new_state = 2;
      count += 1;
    }
  }
  if (new_state != old_state) {
    old_state = new_state;
    lcd.clear();
    lcd.print(count);
    if (ppm)
    {
      lcd.print(" psi");
    }
    else
    {
      lcd.print("%");
    }
    
  }
}

long checkUltrasonic()
{
  long totaldur = 0;
  int durcount = 0;
  for (int i = 0; i < 10; i++)
  {
    long duration;
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    delay(100);
    if (duration < 30000)
    {
      totaldur += duration;
      durcount++;
    }
  }
  return totaldur / durcount;
}

String calculateO2content()
{
  double totpsi = psicount + psi1;

  double o2content = (totpsi*measurement2)/(psicount*(oxygen-nitrogen))-psi1/(psicount*(oxygen-nitrogen))*(.78*nitrogen+.21*oxygen)-nitrogen/(oxygen-nitrogen);
  
  return "o2 content:" + String(o2content,2);
}

void calibrate()
{
  double totpsi = psicount + psi1;

  oxygen = (.78/(1.21*knownO2 - .21))*((measurement2*totpsi-psi1*measurement1)/psicount)+((measurement1*(knownO2-1))/(1.21*knownO2-.21));
  nitrogen = (measurement1-.21*oxygen)/.78;
}

void loop() {
  // read ultra
  if (digitalRead(BUTTON1) == LOW) //get ultrasonic reading
  {
    delay(5);
    while(digitalRead(BUTTON1) == LOW)
    {
      ;
    }
    delay(5);
    if (!calibrated)
    {
      lcd.clear();
      lcd.print("please calibrate");
      lcd.setCursor(0 , 1);
      lcd.print("first. button2");
    }
    else if (measurementcount == 0)
    {
      readultra = true;
      lcd.clear();
      lcd.print("take initial");
      lcd.setCursor(0 , 1);
      lcd.print("ultrasonic read");
      measurementcount++;
    }
    else if (measurementcount == 1)
    {
      measurement1 = checkUltrasonic();
      measurementcount++;
      lcd.clear();
      lcd.print("fill+input new");
      lcd.setCursor(0 , 1);
      lcd.print("psi. button1=done");
    }
    else if (measurementcount == 2)
    {
      psicount = count;
      measurement2 = checkUltrasonic();
      measurementcount++;
      lcd.clear();
      lcd.print(calculateO2content());
      lcd.setCursor(0 , 1);
      lcd.print("button1=restart");
    }
    else if (measurementcount == 3)
    {
      measurementcount = 0;
      firstmessage = true;
    }
  }

  if ((measurementcount == 2))
  {
    checkRotary();
  }

  // calibrate ultra
  if (digitalRead(BUTTON2) == LOW) //get ultrasonic reading
  {
    delay(5);
    while(digitalRead(BUTTON2) == LOW)
    {
      ;
    }
    delay(5);
    readultra = false;
    if (measurementcount == 0)
    {
      ppm = true;
      lcd.clear();
      lcd.print("take initial");
      lcd.setCursor(0 , 1);
      lcd.print("ultrasonic read");
      measurementcount++;
    }
    else if ((measurementcount == 1))
    {
      measurement1 = checkUltrasonic();  
      measurementcount++;
      lcd.clear();
      lcd.print("fill+input new");
      lcd.setCursor(0 , 1);
      lcd.print("psi.button2=next");
    }
    else if ((measurementcount == 2) && m2)
    {
      psicount = count;
      count = 0;
      measurement1 = checkUltrasonic();  
      lcd.clear();
      lcd.print("input o2 concec");
      lcd.setCursor(0 , 1);
      lcd.print("button2=done");
      ppm = false;
      m2 = false;
    }
    else if ((measurementcount == 2) && !m2)
    {
      knownO2 = count / 100.0;
      ppm = true;
      measurement2 = checkUltrasonic();
      measurementcount++;
      calibrate();
      lcd.clear();
      lcd.print("cal finished");
      lcd.setCursor(0 , 1);
      lcd.print("button2=restart");
      calibrated = true;
    }
    else if (measurementcount == 3)
    {
      measurementcount = 0;
      firstmessage = true;
    }
  }

  if ((measurementcount == 0) && firstmessage)
    {
      lcd.clear();
      lcd.print("button1 = read");
      lcd.setCursor(0 , 1);
      lcd.print("button2 = cal");
      firstmessage = false;
      delay(200);
    }

}
