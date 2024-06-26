/**
 * fuzzy_controller.ino
 **/

// eFLL includes
#include <Fuzzy.h>
#include <FuzzyComposition.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzyOutput.h>
#include <FuzzyRule.h>
#include <FuzzyRuleAntecedent.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzySet.h>

// pins
#define LDR A0
#define TRIGGER 4
#define ECHO 5
#define LED 6

// object library
Fuzzy *fuzzy = new Fuzzy();

void setup() {
  // set console and pins
  Serial.begin(9600);
  pinMode(LDR, INPUT);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED, OUTPUT);

  // fuzzy sets
  // distance (0-400cm in Wokwi)
  FuzzySet *small = new FuzzySet(0, 0, 0, 120); //trapizoid MF (a,b,c,d)
  FuzzySet *mid = new FuzzySet(60, 120, 120, 180);
  FuzzySet *big = new FuzzySet(120, 180, 180, 240);
  FuzzySet *verybig = new FuzzySet(180, 240, 400, 400);
  
  // ldr (Potentiometer) (0-1023 in wokwi)
  // ldr (0-100000 lux=> 1015-8 resistance value in wokwi)
  FuzzySet *lowldr = new FuzzySet(0, 0, 0, 100);
  FuzzySet *midldr = new FuzzySet(60, 200, 500, 700);
  FuzzySet *highldr = new FuzzySet(400 ,700, 1015, 1015);
  
  // brightness
  FuzzySet *off = new FuzzySet(0, 0, 0, 0);
  FuzzySet *lowb = new FuzzySet(0, 0, 25, 100);
  FuzzySet *midb = new FuzzySet(55, 100, 155, 200);
  FuzzySet *highb = new FuzzySet(155, 230, 255, 255);

  // variables
  // variable distance with universe 0-400 as input
  FuzzyInput *distance = new FuzzyInput(1);
  distance->addFuzzySet(small);
  distance->addFuzzySet(mid);
  distance->addFuzzySet(big);
  fuzzy->addFuzzyInput(distance);

  // variable ldr with universe 0-1015 as input
  FuzzyInput *ldr = new FuzzyInput(2);
  ldr->addFuzzySet(lowldr);
  ldr->addFuzzySet(midldr);
  ldr->addFuzzySet(highldr);
  fuzzy->addFuzzyInput(ldr); 

  // variable brightness with universe 0-255 as output
  FuzzyOutput *brightness = new FuzzyOutput(1);
  brightness->addFuzzySet(off);
  brightness->addFuzzySet(lowb);
  brightness->addFuzzySet(midb);
  brightness->addFuzzySet(highb);
  fuzzy->addFuzzyOutput(brightness);

  // Rules 1
  // if distance is small and ldr is low then brightness is high
  FuzzyRuleAntecedent *ifDistanceSmallAndLdrIsLow = new FuzzyRuleAntecedent();
  ifDistanceSmallAndLdrIsLow->joinWithAND(small, lowldr); //for Potentiometer highldr , for actual ldr => lowldr
  FuzzyRuleConsequent *thenBrightnessHigh = new FuzzyRuleConsequent();
  thenBrightnessHigh->addOutput(highb); ////for Potentiometer lowb , for actual ldr => highb
  FuzzyRule *fuzzyRule1 = new FuzzyRule(1, ifDistanceSmallAndLdrIsLow, thenBrightnessHigh);
  fuzzy->addFuzzyRule(fuzzyRule1);
  
  // Rules 2
  // if distance is small and ldr is high (light is high) then brightness is off
  FuzzyRuleAntecedent *ifDistanceSmallAndLdrIsHigh = new FuzzyRuleAntecedent();
  ifDistanceSmallAndLdrIsHigh->joinWithAND(small, highldr);
  FuzzyRuleConsequent *thenBrightnessOff = new FuzzyRuleConsequent();
  thenBrightnessOff->addOutput(off);
  FuzzyRule *fuzzyRule2 = new FuzzyRule(2, ifDistanceSmallAndLdrIsHigh, thenBrightnessOff);
  fuzzy->addFuzzyRule(fuzzyRule2);
  
  // Rules 3
  // if distance is mid then brightness is midb
  FuzzyRuleAntecedent *ifDistanceMid = new FuzzyRuleAntecedent();
  ifDistanceMid->joinSingle(mid);
  FuzzyRuleConsequent *thenBrightnessMidb = new FuzzyRuleConsequent();
  thenBrightnessMidb->addOutput(midb);
  FuzzyRule *fuzzyRule3 = new FuzzyRule(3, ifDistanceMid, thenBrightnessMidb);
  fuzzy->addFuzzyRule(fuzzyRule3);
  // Rules 4
  // if distance is big then brightness is low
  FuzzyRuleAntecedent *ifDistanceBig = new FuzzyRuleAntecedent();
  ifDistanceBig->joinSingle(big);
  FuzzyRuleConsequent *thenBrightnessLow = new FuzzyRuleConsequent();
  thenBrightnessLow->addOutput(lowb);
  FuzzyRule* fuzzyRule4 = new FuzzyRule(4, ifDistanceBig, thenBrightnessLow);
  fuzzy->addFuzzyRule(fuzzyRule4);
  
  // Rules 5
  // if distance is verybig then brightness is off
  FuzzyRuleAntecedent *ifDistanceVeryBig = new FuzzyRuleAntecedent();
  ifDistanceVeryBig->joinSingle(verybig);
  FuzzyRule* fuzzyRule5 = new FuzzyRule(5, ifDistanceVeryBig, thenBrightnessOff);
  fuzzy->addFuzzyRule(fuzzyRule5);
}

// returns the distance
int distance() {
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  long pulse = pulseIn(ECHO, HIGH) / 2;
  return pulse * 10 / 292;
}

// returns the brightness
int brightness() {
  return analogRead(LDR);
}
/*
// prints in serial monitor
void print(int dist, int light, int output) {
  Serial.print("distance: ");
  Serial.print(dist);
  Serial.print(" light: ");
  Serial.print(light);
  Serial.print(" => output: ");
  Serial.print(output);
  Serial.println();
}
*/
// main method
void loop() {
  // get distance and light
  int dist = distance();
  int light = brightness();

  // if the inputs are weird, ignore them Miximum value
  if (dist < 0 || dist > 400 || light > 1023) return;

  // fuzzyfication
  fuzzy->setInput(1, dist); // dist as fuzzy input 1 (distance)
  fuzzy->setInput(2, light); // light as fuzzy input 2 (ldr)
  fuzzy->fuzzify();

  // defuzzyfication
  int output = fuzzy->defuzzify(1); // defuzzify fuzzy output 1 (brightness)
  
  analogWrite(LED, output);
  //void print(int dist, int light, int output) {
  Serial.print("distance: ");
  Serial.print(dist);
  Serial.print(" light: ");
  Serial.print(light);
  Serial.print(" => output: ");
  Serial.print(output);
  Serial.println();
  delay(100);
  
}