#define u8 unsigned char

u8 i = 0x00;

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

void I2C(u8 inp1, u8 inp2, u8 inp3, u8 inp4)
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
  TM_WR(inp1);
  TM_ACK();
  TM_WR(inp2);
  TM_ACK();
  TM_WR(inp3);
  TM_ACK();
  TM_WR(inp4);
  TM_ACK();

  // 写命令 开显示
  TM_STOP();
  TM_START();
  TM_WR(0x8f);
  TM_ACK();
  TM_STOP();
}

void printf_bin_8(unsigned char num)
{
  for (int k = 7; k >= 0; k--)
  {
    if (num & (1 << k))
      digitalWrite(LED_BUILTIN, HIGH);
    else
      digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(I2C_SDA, OUTPUT);
  pinMode(I2C_SCL, OUTPUT);
}

void loop()
{
  for (u8 inp1 = 0x00; inp1 <= 0xff; inp1++)
  {
    for (u8 inp2 = 0x00; inp2 <= 0xff; inp2++)
    {
      for (u8 inp3 = 0x00; inp3 <= 0xff; inp3++)
      {
        for (u8 inp4 = 0x00; inp4 <= 0xff; inp4++)
        {
          digitalWrite(LED_BUILTIN, HIGH);
          I2C(inp1,inp2,inp3,inp4);
          printf_bin_8(i);
          i++;
        }
      }
    }
  }
}
