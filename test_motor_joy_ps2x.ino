/**
   ArduinoNa
   Test motor speed control using joy
   วิธีการต่อ สามารถดูได้จากบทความ
*/

#include "motor.h"
#include "joy.h"


void setup() {
  Serial.begin(115200);
  motor_init();
  joy_init();
}

void loop() {

  /**
   * ค่าคันโยก joy ซ้าย แกน x คุมการเลี้ยว
   * ค่าคันโยก joy ซ้าย แกน y คุมการวิ่งตรง
   */
  static int x=0;
  static int y=0;
  static bool event=false;


  /**
   * Joy ใช้ millis() แทน delay 
   * รอ 50 มิลลิวินาทีก่อนการอัพเดทครั้งต่อไป
   * (ถ้ารีบรับค่าจากจอยเร็วเกินไปจะอ่านค่าไม่ได้)
   */
  static uint32_t last_joy = millis();
  if(millis()-last_joy > 50) {
    last_joy = millis();
    ps2x.read_gamepad(false, 0);
    y = ps2x.Analog(PSS_LY);
    x = ps2x.Analog(PSS_LX);  
    Serial.print(x);Serial.print(",");
    Serial.println(y);
  }
  

  /**
   * แปลงค่าจาก [0-255] เป็น [-1,1] ด้วยฟังก์ชัน map
   * forward คือความเร็วแนวตรง + คือไปหน้า - คือถอยหลัง 0 คือหยุด
   * rotate คือความเร็วหมุน + คือตามเข็ม - คือทวนเข็ม 0 คือไม่เลี้ยว
   */
  float forward = -mapf(y, 0, 255, -1.0, 1.0);
  float rotate = mapf(x, 0, 255, -1.0, 1.0);

  
  /**
   * left คือการสั่งการล้อซ้าย [-1,1] เป็นส่วนผสมระหว่างการขับเคลื่อนไปข้างหน้า และการหมุน
   * right คือการสั่งการล้อขวา [-1,1] เป็นส่วนผสมระหว่างการขับเคลื่อนไปข้างหน้า และการหมุน(ตรงข้ามกับล้อซ้าย)
   */
  float left = forward + rotate;
  float right = forward - rotate;
  motor_cmd(left,right);
  
}
