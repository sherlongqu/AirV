#define TM1637_BRIGHTNESS 7

typedef unsigned char u8;
typedef unsigned int u16;

// "void delay(u16 i)" will cause the program to wait for "i" milliseconds
void delay(u16 i)
{
    for (i = i * 16383; i > 0; i--)
        ;
}

sfr P1 = 0x90;

sbit I2C_SCL = P1 ^ 1;
sbit I2C_SDA = P1 ^ 0;

// "void I2C_delay()" will cause the program wait a machine cycle
void I2C_delay()
{
    while (0)
        ;
}

// Start I2C
void I2C_start()
{
    I2C_SCL = 1;
    I2C_SDA = 1;
    I2C_delay();

    I2C_SDA = 0;
}

// I2C bus write 1 byte
void I2C_write_byte(u8 byte_to_write)
{
    u8 i;

    for (i = 0; i < 8; i++)
    {
        I2C_SCL = 0;
        I2C_SDA = byte_to_write & 0x01;
        I2C_delay();

        I2C_SCL = 1;
        I2C_delay();

        I2C_SCL = 0;
        I2C_delay();

        byte_to_write >>= 1;
    }
}

// I2C check response bit
void I2C_check_ack()
{
    I2C_SCL = 0;
    I2C_delay();

    while (I2C_SDA)
    {
        ;
    }

    I2C_SCL = 1;
    I2C_delay();

    I2C_SCL = 0;
}

void I2C_send_ack()
{
    I2C_SDA = 0;
    I2C_delay();

    I2C_SCL = 1;
    I2C_delay();

    I2C_SCL = 0;
    I2C_delay();

    I2C_SDA = 1;
    I2C_delay();
}

void I2C_stop()
{
    I2C_SCL = 0;
    I2C_delay();

    I2C_SDA = 0;
    I2C_delay();

    I2C_SCL = 1;
    I2C_delay();

    I2C_SDA = 1;
}

// For TM1637's code

code u8 TM1637_SEG_TAB[] =
    {
        0x3F // 00111111 0
        ,
        0x06 // 00000110 1
        ,
        0x5B // 01011011 2
        ,
        0x4F // 01001111 3
        ,
        0x66 // 01100110 4
        ,
        0x6d // 01101101 5
        ,
        0x7d // 01111101 6
        ,
        0x07 // 00000111 7
        ,
        0x7F // 01111111 8
        ,
        0x6F // 01101111 9
        ,
        0x77 // 01110111 A
        ,
        0x7C // 01111100 b
        ,
        0x39 // 00111001 C
        ,
        0x5E // 01011110 d
        ,
        0x79 // 01111001 E
        ,
        0x71 // 01110001 F
};

void TM1637_write_command_byte(u8 command)
{
    I2C_start();
    I2C_write_byte(command);
    I2C_check_ack();
    I2C_stop();
}

// bright : 0 - 7
void TM1637_display_switch_and_bright(bit enable_display, u8 bright)
{
    TM1637_write_command_byte(0x80 + (enable_display ? 0x08 : 0x00) + bright);
}

void TM1637_write_display_bytes(u8 first_addr,
                                u8 bytes_to_write[],
                                u8 num_of_bytes)
{
    u8 i;

    I2C_start();
    I2C_write_byte(0x40);
    I2C_check_ack();
    I2C_stop();

    I2C_start();
    I2C_write_byte(0xC0 + first_addr);
    I2C_check_ack();

    for (i = 0; i < num_of_bytes; i++)
    {
        I2C_write_byte(bytes_to_write[i]);
        I2C_check_ack();
    }

    I2C_stop();
}

// addr: 0 - 5
void TM1637_write_display_byte(u8 addr, u8 segments)
{
    I2C_start();
    I2C_write_byte(0x40 + addr);
    I2C_check_ack();
    I2C_stop();

    I2C_start();
    I2C_write_byte(0xC0 + addr);
    I2C_check_ack();

    I2C_write_byte(segments);
    I2C_check_ack();
    I2C_stop();
}

void TM1637_show_digit(u8 pos, u8 digit, bit with_dot)
{
    TM1637_write_display_byte(pos, TM1637_SEG_TAB[digit] | (with_dot ? 0x20 : 0x00));
}

void TM1637_init()
{
    u8 i;

    TM1637_display_switch_and_bright(1, TM1637_BRIGHTNESS);

    for (i = 0; i < 6; i++)
    {
        TM1637_write_display_byte(i, 0xFF);
    }

    delay(200);

    for (i = 0; i < 6; i++)
    {
        TM1637_write_display_byte(i, 0x00);
    }

    delay(200);

    for (i = 0; i < 6; i++)
    {
        TM1637_write_display_byte(i, 0xFF);
    }

    delay(200);

    for (i = 0; i < 6; i++)
    {
        TM1637_write_display_byte(i, 0x00);
    }

    delay(200);
}

void TM1637()
{
    u8 i, j;

    TM1637_init();

    while (1)
    {
        for (i = 0; i < 16; i++)
        {
            for (j = 0; j < 6; j++)
            {
                TM1637_show_digit(j, i, 0);
            }

            delay(1000);
        }
    }
}

void main()
{
    TM1637();
}