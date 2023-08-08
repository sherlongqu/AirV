#define u8 unsigned char

u8 i = 0x00;

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

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  printf_bin_8(i);
  i++;
}
