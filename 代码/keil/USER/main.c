
#include "delay.h"
#include "sys.h"
#include "usart.h"

#include "adc.h"
#include "dma.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"

//void Show_other(void);
//void ShowCH_Voltage(void);
// ���s��ַ?��
char uart2_send_str[100]; 
char uart3_send_str[100]; 
extern uint8_t g_usart_rx_buf[USART_REC_LEN];
uint8_t bussiness_flag = 0;
/*  ����״̬
 *  bit15��      ������ɱ�־
 *  bit14��      ���յ�0x0d
 *  bit13~0��    ���յ�����Ч�ֽ���Ŀ
*/
extern  uint16_t g_usart_rx_sta;
extern uint16_t g_usart3_flag;
// ���͵����ַ�
void USART2_SendChar(char ch)
{
    // �ȴ����ͻ�����Ϊ��
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    // ��Ҫ���͵��ַ�д�뷢�����ݼĴ���
    USART_SendData(USART2, (uint8_t)ch);
}

// ���͵����ַ�
void USART3_SendChar(char ch)
{
    // �ȴ����ͻ�����Ϊ��
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    // ��Ҫ���͵��ַ�д�뷢�����ݼĴ���
    USART_SendData(USART3, (uint8_t)ch);
}

void USART2_SendString(char* str)
{
    while (*str)
    {
        USART2_SendChar(*str++);
    }
}

void USART3_SendString(char* str)  
{  
    while (*str)
    {
        USART3_SendChar(*str++);
    }
}

void esp8266_Config(void){
    // 1. ��λģ�飨��ȴ�������ɣ�
    USART3_SendString("AT+RST\r\n");
    delay_ms(1500); // ��λ��ȴ�2�루ģ������Լ1~2�룩
    
    // 2. ����ATָ�������Ӧ�����賤�ӳ٣�
    USART3_SendString("AT\r\n");    
    delay_ms(1500);  // ����500ms���ɽ�����Ӧ
    
    // 3. �Ͽ���ǰWiFi����δ���ӣ�������Ӧ��
    USART3_SendString("AT+CWQAP\r\n");
    delay_ms(1500);
    
    // 4. ����WiFi����ʱ������㹻�ӳٻ���Ӧ��⣩
    USART3_SendString("AT+CWJAP=\"oneplus\",\"hc123456\"\r\n");
    delay_ms(1500); // �ӳ���5�룬��߳ɹ���
    delay_ms(1500);
    delay_ms(1500);
    delay_ms(1500);
	delay_ms(1500);
    // 5. ���ö����ӣ�������Ӧ��
    USART3_SendString("AT+CIPMUX=1\r\n");    
    delay_ms(1500);
    delay_ms(1500);
    // 6. ��ȡIP��ַ����ȷ��WiFi�����ӣ�
    USART3_SendString("AT+CIFSR\r\n");    
    delay_ms(1500); // ��Ӧ�Ͽ죬���ӳټ���
    delay_ms(1500);
    // 7. ���÷������˿ڣ���������Ϊ8080��
    USART3_SendString("AT+CIPSERVER=1,3030\r\n");
    delay_ms(1500);
       
}         
              


   
int main(void)
 { 
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
	uart2_init(115200);
	uart3_init(115200);
	Adc_Init();		  		//ADC��ʼ��	  
 	dma_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)&AD_Value,N*M);
	printf("*****************system begin***************\r\n");
    esp8266_Config();
    
    while(1)
	{

		while((DMA_GetFlagStatus(DMA1_FLAG_TC1)) == RESET);//ÿ�ΰ����꣬�ٴ�������ó���ѹ
		printf("%d#%d#%d#1000#\r\n",AD_Value[0][0],AD_Value[0][1],AD_Value[0][2]);
		sprintf(uart2_send_str,"%d,%d,%d,",AD_Value[0][0],AD_Value[0][1],AD_Value[0][2]);
        sprintf(uart3_send_str,"%4d#%4d#%4d#1000",AD_Value[0][0],AD_Value[0][1],AD_Value[0][2]);
		USART2_SendString(uart2_send_str);
		//printf("%d#1000#",AD_Value[0][0]);
		delay_ms(5);

		USART3_SendString("AT+CIPSEND=0,19\r\n");
		delay_ms(1000);
		USART3_SendString(uart3_send_str);

	}
}