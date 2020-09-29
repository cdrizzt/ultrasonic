#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    Timer0_Init(1000);

    connect(ui->serial_button,&QPushButton::clicked,this,&Widget::Serial_Open);
}

Widget::~Widget()
{
    delete ui;
}

//定时器初始化
void Widget::Timer0_Init(uint16_t time)
{
    myTime_0 = new QTimer();
    myTime_0->stop();
    myTime_0->setInterval(time);
    connect(myTime_0,SIGNAL(timeout()),this,SLOT(time0_task(void)));
    myTime_0->start();
}

//定时器任务 端口检测
void Widget::time0_task(void)
{
    QStringList port_num;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        port_num += info.portName()+"  "+info.description();
    }
    if(port_num.size()!=serialport_list.size()&&portopen_en==false)
    {
        QStringList same;
        QStringList diff_now,diff_last;
        //比较出相同项及增加项
        for(uint8_t i=0;i<port_num.size();i++)
        {
            bool flag=false;
            for(uint8_t j=0;j<serialport_list.size();j++)
            {
                if(port_num.value(i)==serialport_list.value(j))
                {
                    same<<port_num.value(i);
                    flag=true;
                    break;
                }
            }
            if(flag==false)
            {
                diff_now<<port_num.value(i);
            }
        }
        //比较出减少项并删除
        for(uint8_t i=0;i<serialport_list.size();i++)
        {
            bool flag=false;
            for(uint8_t j=0;j<same.size();j++)
            {
                if(serialport_list.value(i)==same.value(j))
                {
                   flag=true;
                   break;
                }
            }
            if(flag==false)
                ui->com_box->removeItem(ui->com_box->findText(serialport_list.value(i)));
        }
        ui->com_box->addItems(diff_now);
        serialport_list=port_num;
        qDebug()<<port_num;
    }
}

void Widget::Serial_Open(void)
{
    static bool flag = true;
    if(flag)
    {

        if(serialport_list.isEmpty()){
            qDebug()<<"error";
            QMessageBox::warning(this,"ERROR","No serial port connection detected",QMessageBox::Cancel|QMessageBox::Escape|QMessageBox::Default,0);
            return;
        }
        qDebug()<<"open";
        ui->serial_button->setText("close");

        SerialPort = new QSerialPort;
        SerialPort->setPortName(serialport_list.value(0));
        if(SerialPort->open(QIODevice::ReadWrite))
        {
            SerialPort->setBaudRate(QSerialPort::Baud115200);       //波特率
            SerialPort->setBaudRate(QSerialPort::Data8);            //数据位
            SerialPort->setBaudRate(QSerialPort::OneStop);          //停止位
            SerialPort->setParity(QSerialPort::NoParity);           //校验位
            SerialPort->setFlowControl(QSerialPort::NoFlowControl); //控制流

        }
        ui->com_box->setEnabled(false);

    }
    else
    {
        qDebug()<<"close";
        ui->serial_button->setText("open");
        delete SerialPort;  SerialPort = NULL;
        ui->com_box->setEnabled(true);
    }
    flag = !flag;
}
