#include "ultrasonic.h"
#include <QDebug>
ultrasonic::ultrasonic()
{
    data_structre = {0};
    step = 0;
}

bool ultrasonic::data_analysis(QByteArray buf)
{
    for(int i=0;i<buf.size();i++)
    {
        again_this_loop:
        switch (step)
        {
            case 0:         //获取头
            {
                if(uint8_t(buf.at(i))==HEAD_1){
                    data_structre.buff[step] = buf.at(i);
                    step++;
                }
            }break;
            case 1:         //获取头
            {
                if(buf.at(i)==HEAD_2){
                    data_structre.buff[step] = buf.at(i);
                    step++;
                }
            }break;
            case DATA_LEN:        //接校验和
            {
                step = 0;

                uint16_t sum = 0;
                for(int i=0;i<DATA_LEN;i++){
                    if(i==2||i==3){continue;}
                    sum += data_structre.buff[i];
                }

                sum = 0x10000 - sum;

                if(sum == data_structre.sum){       //解码成功
                    for(int i=0;i<ULTRA_NUM;i++)
                    {
                        qDebug()<<data_structre.data[i];
                    }
                    return true;
                }
            }break;

            default:
            {
                data_structre.buff[step] = buf.at(i);
                step++;
                if(step==DATA_LEN) {goto again_this_loop;}
            }break;

        }
    }
    return false;
}

