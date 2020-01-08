/**
   ArduinoNa
   Test motor speed control using joy
   Vector wheel control
   วิธีการต่อ สามารถดูได้จากบทความ 
   www.arduinona.com/article/15/โปรเจค-ใช้-ps2-joy-wireless-เพื่อบังคับรถหรือหุ่นยนต์
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
    x = ps2x.Analog(PSS_RX);  
  }
  

  /**
   * แปลงค่าจากจอยที่โยกซ้าย-ขวา หรือ บน-ล่าง [0-255] เป็น [-1,1] ด้วยฟังก์ชัน mapf
   * yf คือความเร็วแนวตรง + คือไปหน้า - คือถอยหลัง 0 คือหยุด 
   * xf คือความเร็วแนวข้าง + คือขวา - คือซ้าย 0 คือไม่เลี้ยว
   */
  float yf = -mapf(y, 0, 255, -1.0, 1.0); 
  float xf = mapf(x, 0, 255, -0.3, 0.3);  //ความเร็วเลี้ยวกำหนดตรงนี้ เริ่มต้นที่ 0.3 เร็วกว่านี้จะคุมไม่ทันเอานา

  /**
   * ทำให้ vector มีขนาดรวมแกน x และ y ไม่เกิน 1 (normalization) 
   * โดยเก็บทิศทางของ vector ไว้ใน dir 
   * โดย dir[0] คือแกน x และ dir[1] คือแกน y
   */
  float norm = sqrtf(xf*xf + yf*yf);
  if(norm<1.0) norm = 1.0;
  float dir[2] = { xf/norm, yf/norm };
  
  /**
   * left คือการสั่งการล้อซ้าย [-1,1] เป็นส่วนผสมระหว่างการขับเคลื่อนไปข้างหน้า และการหมุน
   * right คือการสั่งการล้อขวา [-1,1] เป็นส่วนผสมระหว่างการขับเคลื่อนไปข้างหน้า และการหมุน(ตรงข้ามกับล้อซ้าย)
   */
  float left = dir[1] + dir[0];
  float right = dir[1] - dir[0];

  /**
   * ทำ normalize ให้ vector มีขนาดเท่ากับ 1 โดยเก็บทิศทางของ vector ไว้
   * ใน wheel โดย wheel[0] คือความเร็วล้อซ้าย wheel[1] คือความเร็วล้อขวา
   */
  float force = sqrtf( left*left + right*right );
  if(force<1.0) force = 1.0;
  float wheel[2] = { left/force, right/force };

  /**
   * สั่งล้อซ้าย และขวาด้วยฟังก์ชัน motor_cmd นิยามใน motor.h ซึ่งจะรับค่า
   * ตั้งแต่ช่วง [-1,1] สำหรับแต่ละล้อ 
   * โดย -1 คือถอยหลังเต็มกำลัง 1 คือเดินหน้าเต็มกำลัง 0 คือหยุด
   */
  motor_cmd(left,right);

//  Serial.print(forward);Serial.print(",");
//  Serial.print(rotate );Serial.print(",");
//  Serial.print(wheel[0]);Serial.print(",");
//  Serial.print(wheel[1]);Serial.print("\n");
  
  
}
