#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QMessageBox>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void Timer0_Init(uint16_t time);       //定时器初始化

private slots:              //槽
    void time0_task(void);                 //定时器回调
    void Serial_Open(void);                //串口打开或关闭

private:
    Ui::Widget *ui;
    bool portopen_en;
    QTimer   *myTime_0;

    QStringList serialport_list;          //串口端口号
    QSerialPort *SerialPort;              //串口
};

#endif // WIDGET_H
