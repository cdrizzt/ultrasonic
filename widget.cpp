#include "widget.h"
#include "ui_widget.h"
#include <QDebug>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    SerialPort = NULL;
    save_file = NULL;
    save_flag = false;
    time_cnt = 0;
    begin = false;
    Timer_Init(myTime_0,time0_task,1000);
    Timer_Init(myTime_1,time1_task,10);

    connect(ui->serial_button,&QPushButton::clicked,this,&Widget::Serial_Open);

}

Widget::~Widget()
{
    delete ui;
}

//定时器初始化
void Widget::Timer_Init(QTimer *timer,void (Widget::*task)(void),uint16_t time)
{
    timer = new QTimer();
    timer->stop();
    timer->setInterval(time);
    connect(timer,&QTimer::timeout,this,task);
    timer->start();
}

//定时器任务
//端口检测
void Widget::time0_task(void)
{
    QStringList port_num;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        port_num += info.portName()+"  "+info.description();
    }
    if(port_num.size()!=serialport_list.size())
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
//串口中断检测
void Widget::time1_task(void)
{
    static uint32_t cnt = 0;
    static uint8_t delay = 0;

    if(begin==true){
        if(++cnt>100){      //数据包计时
            cnt = 0;
            time_cnt++;
            QString str = QString::number(time_cnt);
            ui->label_7->setText(str);
        }
        if(++delay>10){     //数据读取超时

        }
    }
}
//打开/关闭串口
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

        SerialPort = new QSerialPort;
        QString port_set;
        port_set = ui->com_box->currentText();
        port_set = port_set.left(5);
        qDebug()<<port_set;

        SerialPort->setPortName(port_set);
        if(SerialPort->open(QIODevice::ReadWrite))
        {
            SerialPort->setBaudRate(QSerialPort::Baud115200);
            SerialPort->setDataBits(QSerialPort::Data8);
            SerialPort->setStopBits(QSerialPort::OneStop);
            SerialPort->setParity(QSerialPort::NoParity);
            SerialPort->setFlowControl(QSerialPort::NoFlowControl);
        }


        time_cnt = 0;                                           //计数清0
        package_cnt = 0;

        begin = true;
        ui->com_box->setEnabled(false);

        ui->serial_button->setText("close");
        connect(SerialPort,&QSerialPort::readyRead,this,&Widget::data_analysis);

    }
    else
    {
        qDebug()<<"close";
        SerialPort->close();
        begin = false;
        ui->serial_button->setText("open");
        delete SerialPort;  SerialPort = NULL;
        ui->com_box->setEnabled(true);
        disconnect(SerialPort,&QSerialPort::readyRead,this,&Widget::data_analysis);
    }
    flag = !flag;
}
//数据解析
void Widget::data_analysis(void)
{
    QByteArray buf;
    buf = SerialPort->readAll();
    if(!buf.isEmpty())
    {
        qDebug()<<buf;
        if(ultar.data_analysis(buf))           //解码成功
        {

            QString str[6]={0};                    //显示
            str[0] = QString::number(ultar.data_structre.data[0],10);
            ui->ulart_data0->setText(str[0]);

            str[1] = QString::number(ultar.data_structre.data[1],10);
            ui->ulart_data1->setText(str[1]);

            str[2] = QString::number(ultar.data_structre.data[2],10);
            ui->ulart_data2->setText(str[2]);

            str[3] = QString::number(ultar.data_structre.data[3],10);
            ui->ulart_data3->setText(str[3]);

            str[4] = QString::number(ultar.data_structre.data[4],10);
            ui->ulart_data4->setText(str[4]);

            str[5] = QString::number(ultar.data_structre.data[5],10);
            ui->ulart_data5->setText(str[5]);

            package_cnt++;

            QString str_1;
            str_1 = QString::number(package_cnt);
            ui->label_9->setText(str_1);

            //存储文件
            if(save_flag==true)
            {
                for(int i=0;i<6;i++)
                {
                    save_file->write(str[i].toStdString().c_str());
                    i==5?save_file->write("\r\n"):save_file->write(",");

                }
            }

        }

    }
}


void Widget::on_cave_box_clicked(bool checked)
{
    save_flag = checked;
    if(save_flag==true)
    {
        QString path = QFileDialog::getExistingDirectory(this,"选择目录","D:\\qtpractice",\
                                                             QFileDialog::ShowDirsOnly);
        qDebug()<<path;
        QDateTime time = QDateTime::currentDateTime();
        int time_int = time.toTime_t();
        QString filename = "/save_"+QString::number(time_int)+".cvs";

        save_file = new QFile;
        save_file->setFileName(path+filename);

        save_file->open(QIODevice::WriteOnly|QIODevice::Append);
    }
    else
    {
        save_file->close();
        delete save_file;
        save_file = NULL;
    }
}
