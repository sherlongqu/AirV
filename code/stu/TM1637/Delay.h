typedef unsigned int u16;
typedef unsigned char u8;

void Delay200ms()
{
    for (u16 i = 200; i > 0; i--)
    {
        nop_();
    }
}

void Delay1s(){
    for (u16 i = 5; i > 0; i--)
    {
        Delay200ms();
    }
}