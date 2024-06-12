//Master
#include "MPU9250.h"
#include "Wire.h"
#include "Servo.h"


#include <Fuzzy.h>
#include <FuzzyComposition.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzyOutput.h>
#include <FuzzyRule.h>
#include <FuzzyRuleAntecedent.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzySet.h>

#define MPU9250_IMU_ADDRESS 0x68

#define MAGNETIC_DECLINATION 1.63 // To be defined by user
#define INTERVAL_MS_PRINT 10

MPU9250 mpu;
float yaww;
float pitch , roll , yaw , pitch2 , roll2 , yaw2 ;
Servo ServoRoll, ServoPitch, ServoYaw;
unsigned long lastPrintMillis = 0;

// pins
#define LDR A0
#define TRIGGER 4
#define ECHO 5
#define LED 6

// object library
Fuzzy *fuzzy = new Fuzzy();

void setup()
{
  Serial.begin(115200);  // USB üzerinden seri iletişim için
  Serial1.begin(57600); // Nano'dan gelen veriyi almak için

  ServoRoll.attach(2);
  ServoPitch.attach(3);
  ServoYaw.attach(4);
  Wire.begin();

  pinMode(LDR, INPUT);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED, OUTPUT);

  Serial.println("Başlıyor...");

  MPU9250Setting setting;

  // Sample rate must be at least 2x DLPF rate
  setting.accel_fs_sel = ACCEL_FS_SEL::A16G;
  setting.gyro_fs_sel = GYRO_FS_SEL::G1000DPS;
  setting.mag_output_bits = MAG_OUTPUT_BITS::M16BITS;
  setting.fifo_sample_rate = FIFO_SAMPLE_RATE::SMPL_250HZ;
  setting.gyro_fchoice = 0x03;
  setting.gyro_dlpf_cfg = GYRO_DLPF_CFG::DLPF_20HZ;
  setting.accel_fchoice = 0x01;
  setting.accel_dlpf_cfg = ACCEL_DLPF_CFG::DLPF_45HZ;

  mpu.setup(MPU9250_IMU_ADDRESS, setting);

  mpu.setMagneticDeclination(MAGNETIC_DECLINATION);
  mpu.selectFilter(QuatFilterSel::MADGWICK);
  mpu.setFilterIterations(15);

  Serial.println("Kalibrasyon 5 saniye içinde başlayacaktır.");
  Serial.println("Lütfen cihazı sabit bir şekilde düz düzlemde bırakın.");
 // delay(1000);

  Serial.println("Kalibre ediliyor...");
  //mpu.calibrateAccelGyro();

  Serial.println("Manyetometre kalibrasyonu 5 saniye içinde başlayacak.");
  Serial.println("Lütfen cihazı tamamlanana kadar sekiz şeklinde sallayın.");
 // delay(1000);

  Serial.println("Kalibre ediliyor...");
  //mpu.calibrateMag();
yaww =mpu.getYaw();

  Serial.println("Hazır!");

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

void loop()
{
  unsigned long currentMillis = millis();


  if (mpu.update() && currentMillis - lastPrintMillis > INTERVAL_MS_PRINT) {

   pitch=mpu.getPitch();
   roll=mpu.getRoll();
   yaw= (yaww+mpu.getYaw());

    Serial.print("Pitch:");
    Serial.print(roll,1);
    Serial.print("\xC2\xB0\t"); //Print degree symbol
    
    Serial.print("Roll:");
    Serial.print(pitch,1);
    Serial.print("\xC2\xB0\t"); //Print degree symbol
    
    Serial.print("Yaw:");
    Serial.print(yaw,1);
    Serial.print("\xC2\xB0\t\t"); //Print degree symbol

    
  int ServoX = map(roll, -90, 90, 30, -36);
  int ServoY = map(pitch, -40, 40, 138, 58);
  int ServoZ = map(yaw, -180, 0, -80, 100);

  ServoRoll.write(ServoX);
  ServoPitch.write(ServoY);
  ServoYaw.write(ServoZ);

      Serial.print("ServoX:");
    Serial.print(ServoX);
    Serial.print("\xC2\xB0\t"); //Print degree symbol
        Serial.print("ServoY:");
    Serial.print(ServoY);
    Serial.print("\xC2\xB0\t"); //Print degree symbol
        Serial.print("ServoZ:");
    Serial.print(ServoZ);
    Serial.print("\xC2\xB0\t"); //Print degree symbol

  // Nano'dan gelen veriyi oku ve seri monitöre yaz
  if (Serial1.available() ) {

    String data = Serial1.readStringUntil('\n'); // Bir satırı tamamen oku
    int firstComma = data.indexOf(','); // İlk virgülün konumunu bul
    int secondComma = data.indexOf(',', firstComma + 1); // İkinci virgülün konumunu bul

    // Virgülleri kullanarak veriyi ayır
    String pitchStr = data.substring(0, firstComma);
    String rollStr = data.substring(firstComma + 1, secondComma);
    String yawStr = data.substring(secondComma + 1);

    // Stringleri floata dönüştür
    pitch2 = pitchStr.toFloat();
    roll2 = rollStr.toFloat();
    yaw2 = yawStr.toFloat();
    
    // Okunan değerleri seri monitöre yaz
    Serial.print("pitch2: ");
    Serial.print(pitch2,1);
    Serial.print("\xC2\xB0\t");
    Serial.print("roll2: ");
    Serial.print(roll2,1);
    Serial.print("\xC2\xB0\t");
    Serial.print("yaw2: ");
    Serial.print(yaw2,1);
    Serial.print("\xC2\xB0\t");
  }
    Serial.println();
    lastPrintMillis = currentMillis;
  
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
 // Serial.print("distance: ");
 // Serial.print(dist);
 // Serial.print(" light: ");
 // Serial.print(light);
 // Serial.print(" => output: ");
 // Serial.print(output);
 // Serial.println();
 // delay(100);
  

  }
}
