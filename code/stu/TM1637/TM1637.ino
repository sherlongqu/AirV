/*
  This code for TM1637 model
  Develop by sherlongqu
*/

// defind function

#define u8 unsigned char

void TM_START()
{
  // I2C Start
  digitalWrite(I2C_SCL, HIGH);
  digitalWrite(I2C_SDA, HIGH);
  delay(2);
  digitalWrite(I2C_SDA, LOW);
  digitalWrite(I2C_SCL, HIGH);
  delay(2);
}

void TM_WR(u8 onebyte)
{
  // I2C Write
  u8 i;
  for (i = 0; i < 8; i++)
  {
    digitalWrite(I2C_SCL, LOW);
    if (onebyte & 0x01)
    {
      digitalWrite(I2C_SDA, HIGH);
    }
    else
    {
      digitalWrite(I2C_SDA, LOW);
    }
    delay(3);
    onebyte = onebyte >> 1;
    digitalWrite(I2C_SCL, HIGH);
    delay(3);
  }
}

void TM_ACK()
{
  digitalWrite(I2C_SCL, LOW);
  delay(5);
  while (digitalRead(I2C_SDA) == HIGH)
    ;
  digitalWrite(I2C_SCL, HIGH);
  delay(2);
  digitalWrite(I2C_SCL, LOW);
  delay(2);
}

void TM_STOP()
{
  // I2C Stop
  digitalWrite(I2C_SCL, HIGH);
  digitalWrite(I2C_SDA, LOW);
  delay(2);
  digitalWrite(I2C_SDA, HIGH);
  delay(2);
}

void I2C()
{
  // 初始化
  digitalWrite(I2C_SCL, HIGH);
  digitalWrite(I2C_SDA, HIGH);

  // 写命令 设置地址自动加一模式
  TM_START();
  TM_WR(0x40);
  TM_ACK();
  TM_STOP();

  // 写命令 设置数据起始地址
  TM_START();
  TM_WR(0xc0);
  TM_ACK();

  // 写数据
  TM_WR(0x0bf);
  TM_ACK();
  TM_WR(0x3f);
  TM_ACK();
  TM_WR(0x6f);
  TM_ACK();
  TM_WR(0x3f);
  TM_ACK();
  TM_WR(0x6f);
  TM_ACK();

  // 写命令 开显示
  TM_STOP();
  TM_START();
  TM_WR(0x8f);
  TM_ACK();
  TM_STOP();
}

// the setup function runs once when you press reset or power the board
void setup()
{
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(I2C_SDA, OUTPUT);
  pinMode(I2C_SCL, OUTPUT);
}

// the loop function runs over and over again forever
void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  I2C();
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
}
