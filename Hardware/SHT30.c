#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"

#define SHT30_SDA_Pin    GPIO_Pin_11
#define SHT30_SCL_Pin    GPIO_Pin_10
#define SHT30_GPIO       GPIOB 

#define Address          0x44<<1

void SHT30_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(SHT30_GPIO, SHT30_SDA_Pin, (BitAction)(BitValue));
	
}
void SHT30_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(SHT30_GPIO, SHT30_SCL_Pin, (BitAction)(BitValue));
}

uint8_t SHT30_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(SHT30_GPIO,SHT30_SDA_Pin);
	
	return BitValue;
}

void SHT30_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = SHT30_SCL_Pin | SHT30_SDA_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(SHT30_GPIO, &GPIO_InitStructure);
	
	SHT30_W_SCL(1);
	SHT30_W_SDA(1);
}

void SHT30_Start(void)
{
	SHT30_W_SDA(1);
	SHT30_W_SCL(1);
	Delay_us(20);
	SHT30_W_SDA(0);
	Delay_us(20);
	SHT30_W_SCL(0);
	Delay_us(20);
}

void SHT30_Stop(void)
{
	SHT30_W_SDA(0);
	Delay_us(5);
	SHT30_W_SCL(1);
	Delay_us(5);
	SHT30_W_SDA(1);
	Delay_us(5);
}

void SHT30_SendByte(uint8_t Byte)
{
	int i;
	for(i = 0;i < 8; i++)
	{
		SHT30_W_SDA(Byte & (0x80>>i));
		Delay_us(20);
		SHT30_W_SCL(1);
		Delay_us(20);
		SHT30_W_SCL(0);
		Delay_us(20);
	}
}

uint8_t SHT30_ReceiveByte(void)
{
	int i;
	int Byte = 0x00;
	SHT30_W_SDA(1);
	for(i = 0; i < 8; i++)
	{
		SHT30_W_SCL(1);
		if(SHT30_R_SDA() == 1) {Byte |= (0x80>>i);}
		SHT30_W_SCL(0);
	}
	return Byte;
}

void SHT30_SendAck(uint8_t Byte)
{
		SHT30_W_SDA(Byte);
		SHT30_W_SCL(1);
		SHT30_W_SCL(0);
}

uint8_t SHT30_ReceiveAck(void)
{
	uint8_t Ack;
	SHT30_W_SDA(1);
	Delay_us(20);
	SHT30_W_SCL(1);
	Delay_us(20);
  Ack = SHT30_R_SDA();
	Delay_us(20);
	SHT30_W_SCL(0);
	Delay_us(20);
	return Ack;
}

void SHT30_SendCommand(uint8_t addr, uint8_t cmd)
{
	SHT30_Start();
	SHT30_SendByte(Address);
	SHT30_ReceiveAck();
	SHT30_SendByte(addr);
	SHT30_ReceiveAck();
	SHT30_SendByte(cmd);
	SHT30_ReceiveAck();
}

uint8_t SHT30_ReceiveData(uint8_t *Data)
{
	int i;
	SHT30_Start();
	SHT30_SendByte(0x89);
	SHT30_ReceiveAck();
	
	for(i = 0; i < 6; i++)
	{
		Data[i] = SHT30_ReceiveByte();
		if(i == 5)
		{
			SHT30_SendAck(1);
		}
		else
		{
			SHT30_SendAck(0);
		}
	}
	SHT30_Stop();
	return 1;
}

uint8_t SHT30_CRC8(const uint8_t *Data, uint8_t len) {
    uint8_t crc = 0xFF;
    while (len--) {
        crc ^= *Data++;
        for (uint8_t i = 0; i < 8; i++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc <<= 1;
        }
    }
    return crc;
}

uint8_t SHT30_Measure(uint8_t *temp, uint8_t *humi) {
    uint8_t data[6];

    // 1. 发送测量命令 0x2C06（高重复性，时钟拉伸使能）
    SHT30_SendCommand(0x2C, 0x06);

    // 2. 等待测量完成（传感器拉低 SCL，硬件自动等待）
    //    这里可加一小段延时以确保传感器完成，但不是必须
        Delay_ms(20);

    // 3. 读取 6 字节数据
	 // DHT30_ReadData(data);
    if (!SHT30_ReceiveData(data))
		return 0;

    // 4. CRC 校验
    if (SHT30_CRC8(&data[0], 2) != data[2])
        return 0; // 温度 CRC 错误
    if (SHT30_CRC8(&data[3], 2) != data[5])
        return 0; // 湿度 CRC 错误

    // 5. 计算物理值
    uint16_t rawT = (data[0] << 8) | data[1];
    uint16_t rawH = (data[3] << 8) | data[4];
    *temp = (uint8_t)(-45.0f + 175.0f * (rawT / 65535.0f));
    *humi  = (uint8_t)(100.0f * (rawH / 65535.0f));

    return 1;
}

uint8_t aaa(void)
{
	uint8_t i;
	SHT30_Start();
	SHT30_SendByte(0x88);
	i = SHT30_ReceiveAck();
	SHT30_Stop();
	return i;
}
