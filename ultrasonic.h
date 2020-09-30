#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#define HEAD_1      0xA5
#define HEAD_2      0x5A
#define HEAD_LEN    (2)

#define SUM_LEN     (2)

#define ULTRA_NUM (6)
#define ULTAR_DATA_LEN (2)



#define DATA_LEN ((ULTRA_NUM*ULTAR_DATA_LEN)+HEAD_LEN+SUM_LEN)


#include <QWidget>

typedef union{
    uint8_t buff[DATA_LEN];
    struct __attribute__((packed)){
        uint8_t head_1;             //头1
        uint8_t head_2;             //头2
        uint16_t sum;               //校验和高
        uint16_t data[DATA_LEN-SUM_LEN-HEAD_LEN];           //数据  超声波0-5
    };
}DATA_STRUCTURE;

class ultrasonic
{


public:
    int step;
    ultrasonic();
    bool data_analysis(QByteArray buf);
    DATA_STRUCTURE data_structre;   //数据解码


};

#endif // ULTRASONIC_H
