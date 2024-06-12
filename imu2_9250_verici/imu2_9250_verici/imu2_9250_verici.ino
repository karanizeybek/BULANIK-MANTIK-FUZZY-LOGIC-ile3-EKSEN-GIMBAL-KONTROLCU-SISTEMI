//Slave
#include "MPU9250.h"
#include "Wire.h"

#define MPU9250_IMU_ADDRESS 0x68

#define MAGNETIC_DECLINATION 1.63 // To be defined by user
#define INTERVAL_MS_PRINT 50

MPU9250 mpu;

unsigned long lastPrintMillis = 0;

void setup()
{
  Serial.begin(57600);
  Wire.begin();

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
  delay(5000);

  Serial.println("Kalibre ediliyor...");
  mpu.calibrateAccelGyro();

  Serial.println("Manyetometre kalibrasyonu 5 saniye içinde başlayacak.");
  Serial.println("Lütfen cihazı tamamlanana kadar sekiz şeklinde sallayın.");
  delay(5000);

  Serial.println("Kalibre ediliyor...");
  mpu.calibrateMag();
  
  Serial.println("Hazır!");
  delay(1000);
}

void loop()
{
  unsigned long currentMillis = millis();


  if (mpu.update() && currentMillis - lastPrintMillis > INTERVAL_MS_PRINT) {
    float pitch=mpu.getPitch();
    float roll=mpu.getRoll();
    float yaw=mpu.getYaw();

    // Veriyi gönder
    Serial.print(pitch,1);
    Serial.print(",");
    Serial.print(roll,1);
    Serial.print(",");
    Serial.println(yaw,1);
  

    lastPrintMillis = currentMillis;
  }
}
