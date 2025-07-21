#include "./serial_screen/serial_screen.h"

                                                            //100����(10����λ)


/* ҳ�棯�ؼ� ID ���� */
#define PAGE_MEASURE      1
#define CTRL_BTN_SET_DUTY 2   // ���衰����ռ�ձȡ���ť�� Control ID
#define CTRL_TEXT_DUTY    5   // �ı���� duty_set �� Control ID
#define CTRL_BTN_MEASURE_DUTY 3   // ��ռ�ձȲ�������ť
#define CTRL_TEXT_DUTY_RESULT 6   // ��ʾ����������ı��ؼ�
#define CTRL_BIN_MEASURE_THD  4   // "ʧ��Ȳ���"��ť
#define CTRL_TEXT_THD_RESULT 7   // ��ʾ����������ı��ؼ�

#define PAGE_LISSAJOUS    2
#define CTRL_BTN_PLL_MULTIPLY 3   // �����໷��Ƶ����ť
#define CTRL_TEXT_PLL_MULTIPLY 5   // �����໷��Ƶ���󶨱�Ƶֵ���ı��ؼ�
#define CTRL_BIN_LISSAJOUS_DISPLAY 4   // ��������ͼ�񡱵İ�ť�ؼ�
#define CTRL_BIN_LISSAJOUS_STOP    6   // ��������ͼ�񡱵�ֹͣ��ť�ؼ�
#define CTRL_BIN_PHASE_DETECT      7   // ����λ��⡱��ť�ؼ�
#define CTRL_TEXT_PHASE_VAL        8   // ����λ��⡱����λֵ���ı��ؼ�


volatile uint32  timer_tick_count = 0;                                               //��ʱ������

uint8  cmd_buffer[CMD_MAX_SIZE];                                                     //ָ���
static uint16 current_screen_id = 0;                                                 //��ǰ����ID
static int32 test_value = 0;                                                         //����ֵ
static uint8 update_en = 0;                                                          //���±��
void UpdateUI(void);                                                                 //����UI����

/* MCU �˴洢��ռ�ձ�����ֵ */
static uint16_t duty_set = 0;

/*! 
*  \brief  ��Ϣ��������
*  \param msg ��������Ϣ
*  \param size ��Ϣ����
*/
void ProcessMessage( PCTRL_MSG msg, uint16 size )
{
    uint8 cmd_type = msg->cmd_type;                                                  //ָ������
    uint8 ctrl_msg = msg->ctrl_msg;                                                  //��Ϣ������
    uint8 control_type = msg->control_type;                                          //�ؼ�����
    uint16 screen_id = PTR2U16(&msg->screen_id);                                     //����ID
    uint16 control_id = PTR2U16(&msg->control_id);                                   //�ؼ�ID
    uint32 value = PTR2U32(msg->param);                                              //��ֵ


    switch(cmd_type)
    {  
    case NOTIFY_TOUCH_PRESS:                                                        //����������
    case NOTIFY_TOUCH_RELEASE:                                                      //�������ɿ�
        NotifyTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4)); 
        break;                                                                    
    case NOTIFY_WRITE_FLASH_OK:                                                     //дFLASH�ɹ�
        NotifyWriteFlash(1);                                                      
        break;                                                                    
    case NOTIFY_WRITE_FLASH_FAILD:                                                  //дFLASHʧ��
        NotifyWriteFlash(0);                                                      
        break;                                                                    
    case NOTIFY_READ_FLASH_OK:                                                      //��ȡFLASH�ɹ�
        NotifyReadFlash(1,cmd_buffer+2,size-6);                                     //ȥ��֡ͷ֡β
        break;                                                                    
    case NOTIFY_READ_FLASH_FAILD:                                                   //��ȡFLASHʧ��
        NotifyReadFlash(0,0,0);                                                   
        break;                                                                    
    case NOTIFY_READ_RTC:                                                           //��ȡRTCʱ��
        NotifyReadRTC(cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7],cmd_buffer[8]);
        break;
    case NOTIFY_CONTROL:
        {
            if(ctrl_msg==MSG_GET_CURRENT_SCREEN)                                    //����ID�仯֪ͨ
            {
                NotifyScreen(screen_id);                                            //�����л������ĺ���
            }
            else
            {
                switch(control_type)
                {
                case kCtrlButton:                                                   //��ť�ؼ�
                    NotifyButton(screen_id,control_id,msg->param[1]);                  
                    break;                                                             
                case kCtrlText:                                                     //�ı��ؼ�
                    NotifyText(screen_id,control_id,msg->param);                       
                    break;                                                             
                case kCtrlProgress:                                                 //�������ؼ�
                    NotifyProgress(screen_id,control_id,value);                        
                    break;                                                             
                case kCtrlSlider:                                                   //�������ؼ�
                    NotifySlider(screen_id,control_id,value);                          
                    break;                                                             
                case kCtrlMeter:                                                    //�Ǳ�ؼ�
                    NotifyMeter(screen_id,control_id,value);                           
                    break;                                                             
                case kCtrlMenu:                                                     //�˵��ؼ�
                    NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);      
                    break;                                                              
                case kCtrlSelector:                                                 //ѡ��ؼ�
                    NotifySelector(screen_id,control_id,msg->param[0]);                
                    break;                                                              
                case kCtrlRTC:                                                      //����ʱ�ؼ�
                    NotifyTimer(screen_id,control_id);
                    break;
                default:
                    break;
                }
            } 
            break;  
        } 
    default:
        break;
    }
}

/*! 
*  \brief  �����л�֪ͨ
*  \details  ��ǰ����ı�ʱ(�����GetScreen)��ִ�д˺���
*  \param screen_id ��ǰ����ID
*/
void NotifyScreen(uint16 screen_id)
{
    //TODO: ����û�����
    current_screen_id = screen_id;                                                   //�ڹ��������п��������л�֪ͨ����¼��ǰ����ID

    //��������3����һ����ť
    if(screen_id == 3)
    {
        SetButtonValue(3,1,1);
    }
    //���������Զ�����GIF
    if(current_screen_id == 9)
    {
        AnimationStart(9,1);                                                         //������ʼ����
    }

    //������ά��ҳ�����ɶ�ά��
    if(current_screen_id==14)                                   
    {
        //��ά��ؼ���ʾ�����ַ�ʱ����Ҫת��ΪUTF8���룬
        //ͨ����ָ�����֡���ת����www.gz-dc.com�� ���õ��ַ�����������
        uint8 dat[] = {0x77,0x77,0x77,0x2E,0x67,0x7A,0x2D,0x64,0x63,0x2E,0x63,0x6F,0x6D};
        SetTextValue(14,1,dat);                                                      //���Ͷ�ά���ַ�����                     
    }

    //���ݼ�¼��ʾ
    if(current_screen_id == 15)
    {
        Record_SetEvent(15,1,0,0);  
        Record_SetEvent(15,1,1,0);
        Record_SetEvent(15,1,2,0);
        Record_SetEvent(15,1,3,0);
        Record_SetEvent(15,1,4,0);
        Record_SetEvent(15,1,5,0);
        Record_SetEvent(15,1,6,0);
        Record_SetEvent(15,1,7,0);
        HAL_Delay(2000);                                                              //��ʱ����
        Record_ResetEvent(15,1,0,0);
        Record_ResetEvent(15,1,1,0);
        Record_ResetEvent(15,1,2,0);

    }
    //�������ֻ����Զ�����
    if(current_screen_id == 18)   
    {
        uint8 buffer[19] = {0x94,0x61 ,0x3A ,0x2F ,0x73 ,0x6F ,0x75 ,0x6E ,0x64 ,0x73 ,0x2F , 0x30 ,0x31 ,0x2E ,0x6D ,0x70 ,0x33};
        SetButtonValue(18,3,1);
        PlayMusic(buffer);                                                           //��������
    }
}

/*! 
*  \brief  ���������¼���Ӧ
*  \param press 1���´�������3�ɿ�������
*  \param x x����
*  \param y y����
*/
void NotifyTouchXY(uint8 press,uint16 x,uint16 y)
{ 
    //TODO: ����û�����
}


/*! 
*  \brief  ��������
*/ 
void UpdateUI(void)
{
    if (current_screen_id == PAGE_LISSAJOUS) {
        Lissajous_Graph_Display(lissajous_graph_state);
    }
}

/*! 
*  \brief  ��ť�ؼ�֪ͨ
*  \details  ����ť״̬�ı�(�����GetControlValue)ʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param state ��ť״̬��0����1����
*/
void NotifyButton(uint16 screen_id, uint16 control_id, uint8  state)
{
    // ������ռ�ձȡ���ť������
    if (screen_id == PAGE_MEASURE && control_id == CTRL_BTN_SET_DUTY && state == 1) {
        GetControlValue(PAGE_MEASURE, CTRL_TEXT_DUTY);
    }
    // ���ǡ�����ռ�ձȡ���ť������
    else if (screen_id == PAGE_MEASURE && control_id == CTRL_BTN_MEASURE_DUTY)
    {
        float measured_duty = 0;
        MeasureDuty(&measured_duty);
        char duty_str[8];
        snprintf(duty_str, sizeof(duty_str), "%.1f%%", measured_duty * 100.0f);
        SetTextValue(PAGE_MEASURE, CTRL_TEXT_DUTY_RESULT, (uint8_t*)duty_str);
    }
    // ���ǡ�ʧ��Ȳ�������ť������
    else if (screen_id == PAGE_MEASURE && control_id == CTRL_BIN_MEASURE_THD)
    {
        float measured_thd = 0;
        MeasureTHD(&measured_thd);
        char thd_str[8];
        snprintf(thd_str, sizeof(thd_str), "%.1f%%", measured_thd);
        SetTextValue(PAGE_MEASURE, CTRL_TEXT_THD_RESULT, (uint8_t*)thd_str);
    }
    // ���ǡ����໷��Ƶ����ť������
    else if (screen_id == PAGE_LISSAJOUS && control_id == CTRL_BTN_PLL_MULTIPLY)
    {
        GetControlValue(PAGE_LISSAJOUS, CTRL_TEXT_PLL_MULTIPLY);
    }
    // ���ǡ�������ͼ�񡱰�ť������
    else if (screen_id == PAGE_LISSAJOUS && control_id == CTRL_BIN_LISSAJOUS_DISPLAY)
    {
        Lissajous_Graph_Start();
    }
    // ���ǡ���ͣ����ť������
    else if (screen_id == PAGE_LISSAJOUS && control_id == CTRL_BIN_LISSAJOUS_STOP)
    {
        Lissajous_Graph_Stop();
    }
    // ���ǡ���λ��⡱��ť������
    else if (screen_id == PAGE_LISSAJOUS && control_id == CTRL_BIN_PHASE_DETECT)
    {
        
    }
    
}

/*! 
*  \brief  �ı��ؼ�֪ͨ
*  \details  ���ı�ͨ�����̸���(�����GetControlValue)ʱ��ִ�д˺���
*  \details  �ı��ؼ����������ַ�����ʽ�·���MCU������ı��ؼ������Ǹ���ֵ��
*  \details  ����Ҫ�ڴ˺����н��·��ַ�������ת�ظ���ֵ��
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param str �ı��ؼ�����
*/
void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str)
{
    if (screen_id == PAGE_MEASURE && control_id == CTRL_TEXT_DUTY) {
        float duty_value = 0.0f;
        sscanf((const char*)str, "%f", &duty_value);
        if (duty_value < 0) duty_value = 0;
        if (duty_value > 100) duty_value = 100;

        SetDuty((uint16_t)duty_value);
        SetTextFloat(PAGE_MEASURE, CTRL_TEXT_DUTY, duty_value, 0, 1);
    }
    // �����ı��ؼ��߼��ɼ�������
    else if (screen_id == PAGE_LISSAJOUS && control_id == CTRL_TEXT_PLL_MULTIPLY)
    {
        float ppl_value = 0;
        sscanf((const char*)str, "%d", &ppl_value);
        SetPLLMultiply((uint16_t)ppl_value);
        SetTextInt32(PAGE_LISSAJOUS, CTRL_TEXT_PLL_MULTIPLY, (uint32_t)ppl_value, 0, 0);
    }
}

/*!                                                                              
*  \brief  �������ؼ�֪ͨ                                                       
*  \details  ����GetControlValueʱ��ִ�д˺���                                  
*  \param screen_id ����ID                                                      
*  \param control_id �ؼ�ID                                                     
*  \param value ֵ                                                              
*/                                                                              
void NotifyProgress(uint16 screen_id, uint16 control_id, uint32 value)           
{  
    //TODO: ����û�����   
}                                                                                

/*!                                                                              
*  \brief  �������ؼ�֪ͨ                                                       
*  \details  ���������ı�(�����GetControlValue)ʱ��ִ�д˺���                  
*  \param screen_id ����ID                                                      
*  \param control_id �ؼ�ID                                                     
*  \param value ֵ                                                              
*/                                                                              
void NotifySlider(uint16 screen_id, uint16 control_id, uint32 value)             
{                                                             
   
}

/*! 
*  \brief  �Ǳ�ؼ�֪ͨ
*  \details  ����GetControlValueʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param value ֵ
*/
void NotifyMeter(uint16 screen_id, uint16 control_id, uint32 value)
{
    //TODO: ����û�����
}

/*! 
*  \brief  �˵��ؼ�֪ͨ
*  \details  ���˵���»��ɿ�ʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param item �˵�������
*  \param state ��ť״̬��0�ɿ���1����
*/
void NotifyMenu(uint16 screen_id, uint16 control_id, uint8 item, uint8 state)
{
    //TODO: ����û�����
}

/*! 
*  \brief  ѡ��ؼ�֪ͨ
*  \details  ��ѡ��ؼ��仯ʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param item ��ǰѡ��
*/
void NotifySelector(uint16 screen_id, uint16 control_id, uint8  item)
{
  
}

/*! 
*  \brief  ��ʱ����ʱ֪ͨ����
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*/
void NotifyTimer(uint16 screen_id, uint16 control_id)
{

}

/*! 
*  \brief  ��ȡ�û�FLASH״̬����
*  \param status 0ʧ�ܣ�1�ɹ�
*  \param _data ��������
*  \param length ���ݳ���
*/
void NotifyReadFlash(uint8 status,uint8 *_data,uint16 length)
{
    //TODO: ����û�����
}

/*! 
*  \brief  д�û�FLASH״̬����
*  \param status 0ʧ�ܣ�1�ɹ�
*/
void NotifyWriteFlash(uint8 status)
{
    //TODO: ����û�����
}

/*! 
*  \brief  ��ȡRTCʱ�䣬ע�ⷵ�ص���BCD��
*  \param year �꣨BCD��
*  \param month �£�BCD��
*  \param week ���ڣ�BCD��
*  \param day �գ�BCD��
*  \param hour ʱ��BCD��
*  \param minute �֣�BCD��
*  \param second �루BCD��
*/
void NotifyReadRTC(uint8 year,uint8 month,uint8 week,uint8 day,uint8 hour,uint8 minute,uint8 second)
{
    uint8 years;
    uint8 months;
    uint8 weeks;
    uint8 days;
    uint8 hours;
    uint8 minutes;
    uint8 sec;
    
    sec =(0xff & (second>>4))*10 +(0xf & second);                                    //BCD��תʮ����
    years =(0xff & (year>>4))*10 +(0xf & year);                                      
    months =(0xff & (month>>4))*10 +(0xf & month);                                     
    weeks =(0xff & (week>>4))*10 +(0xf & week);                                      
    days =(0xff & (day>>4))*10 +(0xf & day);                                      
    hours =(0xff & (hour>>4))*10 +(0xf & hour);                                       
    minutes =(0xff & (minute>>4))*10 +(0xf & minute);                                      
                                      
}