/**
   ArduinoNa
   Motor 2channel utility function
*/

//ขาสัญญาณมอเตอร์ซ้าย
uint8_t MLftDirAPin = 7;
uint8_t MLftDirBPin = 8;
uint8_t MLftSpdPin = 9;
bool  MLftRev = false;    //กลับทิศทางล้อซ้าย

//ขาสัญญาณมอเตอร์ขวา
uint8_t MRgtDirAPin = 12;
uint8_t MRgtDirBPin = 11;
uint8_t MRgtSpdPin = 10;
bool  MRgtRev = true;     //กลับทิศทางล้อขวา

#define MIN_PWM_SPD 40    //ตั้งค่าความเร็ว PWM [0-254] ต่ำสุดที่ล้อเริ่มหมุน
#define MAX_PWM_SPD 150   //ตั้งค่าความเร็ว PWM [0-254] สูงสุดที่จะใช้
#define DEADBAND_PWM 0.1  //ตั้งค่าช่วงไร้การสั่งการ

//ฟังก์ชันทำ deadband สำหรับตัดค่าที่ต่ำกว่า deadband ให้กลายเป็น 0 สำหรับช่วง [-1,1]
float applyDeadband(float value, float d) {
  float out=0;
  if (value>d) 
    out = (value-d)/(1-d);
  else if (value>-d) 
    out = 0;
  else 
    out = (value+d)/(1-d);
  return out;
}

//ฟังก์ชัน map ค่าแบบทศนิยม
double mapf(double val, double in_min, double in_max, double out_min, double out_max) {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}



//ฟังก์ชันดูเครื่องหมาย >0 ให้ค่าเป็น 1 , <0 ให้ค่าเป็น -1
template <typename type>
type sign(type value) {
    return type((value>0)-(value<0));
}

//ฟังก์ชันเริ่มการตั้งค่ามอเตอร์ ให้รันใน Setup()
void motor_init() {
    pinMode(MLftDirAPin, OUTPUT);
    pinMode(MLftDirBPin, OUTPUT);
    pinMode(MRgtDirAPin, OUTPUT);
    pinMode(MRgtDirBPin, OUTPUT);
    Serial.println("Motor inited");
}

/**
 * ฟังก์ชันขับมอเตอร์โดยดูจากความเร็วที่ให้มา หาก speed เป็น + ให้ทิศทางไปข้างหน้า - คือถอยหลัง 0 คือหยุด
 * โดยความเร็วมีค่าตั้งแต่ -1 ถึง 1 สำหรับแต่ละล้อ
 * @param leftspd [-1.0, 1.0]
 * @param rightspd [-1.0, 1.0]
 */
void motor_cmd(float leftspd, float rightspd) {

    //ตรวจสอบความเร็วว่าสั่งการ spd >0 ล้อไปหน้า, <0 ล้อถอยหลัง, 0 หยุด
    digitalWrite( MLftDirAPin, (MLftRev ? sign(leftspd)>0 : sign(leftspd)<0) );
    digitalWrite( MLftDirBPin, (MLftRev ? sign(leftspd)<0 : sign(leftspd)>0) );
    digitalWrite( MRgtDirAPin, (MRgtRev ? sign(rightspd)>0 : sign(rightspd)<0) );
    digitalWrite( MRgtDirBPin, (MRgtRev ? sign(rightspd)<0 : sign(rightspd)>0) );


    /**
     * ทำความเร็วให้เป็น + ด้วยคำสั่ง function absolute  (fabs) จากนั้นลิมิตค่าให้ไม่เกิน
     */
    float Lout = constrain( fabs(leftspd*1000), 0, 1000)/1000.0;
    float Rout = constrain( fabs(rightspd*1000), 0, 1000)/1000.0;

    /**
     *        PWM OUT(Duty of cycle 0-255)
     * 255.0  |
     *        }      /
     *        }     /
     *        }    /
     * MIN_SPD|___/______________ 1.0 _> Control signal
     *            deadband
     */

    Lout = applyDeadband(Lout, DEADBAND_PWM);
    Rout = applyDeadband(Rout, DEADBAND_PWM);

    
    if(Lout>0) {
      Lout = mapf(Lout, 0, 1.0, MIN_PWM_SPD, MAX_PWM_SPD);
    }
    if(Rout>0) {
      Rout = mapf(Rout, 0, 1.0, MIN_PWM_SPD, MAX_PWM_SPD);
    }



    analogWrite(MLftSpdPin, Lout);
    analogWrite(MRgtSpdPin, Rout);
}
