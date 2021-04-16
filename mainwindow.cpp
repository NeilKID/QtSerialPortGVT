#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //连接读取串口信号和槽
    QObject::connect(&serial, &QSerialPort::readyRead, this, &MainWindow::serialPort_readyRead);
    //发送按键失能
    ui->sendButton->setEnabled(false);
    //波特率默认选择下拉第三项：9600
    ui->baudrateBox->setCurrentIndex(3);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//读取串口数据
void MainWindow::serialPort_readyRead()
{
    //从接收缓冲区中读取数据
    QByteArray buffer = serial.readAll();
    QString recv = "#";
//    QString recv = buffer.toHex(':').toUpper().data();//测试是否接收数据

    //初始标识符识别
    for (int count = 0; count < buffer.length();count++)
    {
        if (buffer[count] == '#')
        {
            /*
            //接收4字节经度数据并转为float数据（保留2位小数）显示
            char lonCh[4];
            for(int i=0;i<4;i++)
            {
                lonCh[i] = buffer[i+1];
            }
            float lonFl = 0.0;
            recv_float_char(lonFl,lonCh);
            recv.append(QString::number(lonFl,'f',2));
            recv.append(' ');
            //接收4字节经度数据并转为float数据（保留2位小数）显示
            char latCh[4];
            for(int i=0;i<4;i++)
            {
                latCh[i] = buffer[i+5];
            }
            float latFl = 0.0;
            recv_float_char(latFl,latCh);
            recv.append(QString::number(latFl,'f',2));
            recv.append(' ');
            //接收2字节航向角并转为int数据显示
            char yawCh[2];
            for(int i=0;i<2;i++)
            {
                yawCh[i] = buffer[i+9];
            }
            unsigned short yawUS = 0;
            recv_ushort_char(yawUS,yawCh);
            recv.append(QString::number(yawUS));
            recv.append(' ');
            //接收2字节速度并转为int数据显示
            char vecCh[2];
            for(int i=0;i<2;i++)
            {
                vecCh[i] = buffer[i+11];
            }
            unsigned short vecUS = 0;
            recv_ushort_char(vecUS,vecCh);
            recv.append(QString::number(vecUS));
            recv.append(' ');
            //接收1字节定位模式并以字符串显示
            char locMoCh = buffer[13];
            recv.append(locMoCh);
            */

            //延迟测试，解析发送端数据
            char recvSequCh = buffer[1];
            recv.append(QString(recvSequCh));
            recv.append(' ');
            //接收2字节转角并以字符串显示
            char recvSteerAngleCh[2];
            for(int i=0;i<2;i++)
            {
                recvSteerAngleCh[i]=buffer[i+2];
            }
            signed short recvSteerAngleSh = 0;
            recv_short_char(recvSteerAngleSh,recvSteerAngleCh);
            recv.append(QString::number(recvSteerAngleSh));
            recv.append(' ');
            //接收2字节速度并以字符串显示
            char recvVelocityCh[2];
            for(int i=0;i<2;i++)
            {
                recvVelocityCh[i]=buffer[i+4];
            }
            unsigned short recvVelocityUS = 0;
            recv_ushort_char(recvVelocityUS,recvVelocityCh);
            recv.append(QString::number(recvVelocityUS));
            recv.append(' ');
            //接收2字节发送时间秒并以字符串显示
            char recvSecondStartCh[4];
            for(int i=0;i<4;i++)
            {
                recvSecondStartCh[i]=buffer[i+6];
            }
            int recvSecondStart = 0;
            recv_int_char(recvSecondStart,recvSecondStartCh);
            recv.append(QString::number(recvSecondStart));
            recv.append(':');
            //接收2字节发送时间毫秒并以字符串显示
            char recvMsecStartCh[4];
            for(int i=0;i<4;i++)
            {
                recvMsecStartCh[i]=buffer[i+10];
            }
            int recvMsecStart = 0;
            recv_int_char(recvMsecStart,recvMsecStartCh);
            recv.append(QString::number(recvMsecStart));
            recv.append(' ');

            //接收2字节接收时间秒并以字符串显示
            QTime current_time_end = QTime::currentTime();
            int secondEnd = current_time_end.second();
            recv.append(QString::number(secondEnd));
            recv.append(':');
            //接收2字节接收时间毫秒并以字符串显示
            int msecEnd = current_time_end.msec();
            recv.append(QString::number(msecEnd));
            recv.append(' ');

            //计算通讯延迟时间
            int delayTimeSecond = secondEnd - recvSecondStart;
            recv.append(QString::number(delayTimeSecond));
            recv.append(':');
            int delayTimeMsec = msecEnd - recvMsecStart;
            recv.append(QString::number(delayTimeMsec));
        }
    }

    //从界面中读取以前收到的数据
    ui->recvTextEdit->append(recv);

    //获得接收串口读取的ByteArray数据长度，存储为int，以字符串形式显示
    int recvLengthNum = buffer.size();
//    int recvLengthNum = sizeof(short); //测试short类型占用字节
    QString recvLength = QString::number(recvLengthNum);
    ui->recvTextLength->append(recvLength);
}

//点击检测串口信息，显示目前可用串口
void MainWindow::on_searchButton_clicked()
{
    ui->portNameBox->clear();
    QList<QSerialPortInfo> strlist=QSerialPortInfo::availablePorts();
    QList<QSerialPortInfo>::const_iterator iter;
    for(iter=strlist.constBegin();iter!=strlist.constEnd();++iter)
    {
        ui->portNameBox->addItem((*iter).portName());
    }
}

//点击打开串口
void MainWindow::on_openButton_clicked()
{
    if(ui->openButton->text()==QString::fromUtf8("打开串口"))
    {
        //设置串口名
        serial.setPortName(ui->portNameBox->currentText());
        //设置波特率
        serial.setBaudRate(ui->baudrateBox->currentText().toInt());
        //设置数据位数
        switch(ui->dataBitsBox->currentText().toInt())
        {
        case 8:
            serial.setDataBits(QSerialPort::Data8);
            break;
        case 7:
            serial.setDataBits(QSerialPort::Data7);
            break;
        case 6:
            serial.setDataBits(QSerialPort::Data6);
            break;
        case 5:
            serial.setDataBits(QSerialPort::Data5);
            break;
        default:
            break;
        }
        //设置奇偶校验
        switch(ui->parityBox->currentIndex())
        {
        case 0:
            serial.setParity(QSerialPort::NoParity);
            break;
        case 1:
            serial.setParity(QSerialPort::EvenParity);
            break;
        case 2:
            serial.setParity(QSerialPort::OddParity);
            break;
        default:
            break;
        }
        //设置停止位
        switch(ui->stopBitsBox->currentIndex())
        {
        case 0:
            serial.setStopBits(QSerialPort::OneStop);
            break;
        case 1:
            serial.setStopBits(QSerialPort::TwoStop);
        case 2:
            serial.setStopBits(QSerialPort::OneAndHalfStop);
            break;
        default: break;
        }
        //设置流控制
        switch (ui->flowBox->currentIndex())
        {
        case 0:
            serial.setFlowControl(QSerialPort::NoFlowControl);
            break;
        case 1:
            serial.setFlowControl(QSerialPort::HardwareControl);
            break;
        case 2:
            serial.setFlowControl(QSerialPort::SoftwareControl);
            break;
        }
        //打开串口
        if(!serial.open(QIODevice::ReadWrite))
        {
           QMessageBox::about(NULL, QString::fromUtf8("提示 "), QString::fromUtf8("无法打开串口！"));
           return;
        }
        //下拉菜单控件失能
        ui->portNameBox->setEnabled(false);
        ui->baudrateBox->setEnabled(false);
        ui->dataBitsBox->setEnabled(false);
        ui->parityBox->setEnabled(false);
        ui->stopBitsBox->setEnabled(false);
        ui->flowBox->setEnabled(false);
        ui->openButton->setText(QString::fromUtf8("关闭串口"));
        //发送按键使能
        ui->sendButton->setEnabled(true);
    }
    else
    {
        //关闭串口
        serial.close();
        //下拉菜单控件使能
        ui->portNameBox->setEnabled(true);
        ui->baudrateBox->setEnabled(true);
        ui->dataBitsBox->setEnabled(true);
        ui->parityBox->setEnabled(true);
        ui->stopBitsBox->setEnabled(true);
        ui->flowBox->setEnabled(true);
        ui->openButton->setText(QString::fromUtf8("打开串口"));
        //发送按键失能
        ui->sendButton->setEnabled(false);
    }
}

//点击发送数据
void MainWindow::on_sendButton_clicked()
{
    //从编辑栏读取用户写入的转角与速度，分别存为unsigned short与float
    QString strSendSw = ui->sendSwTextEdit->toPlainText();
    QString strSendVec = ui->sendVecTextEdit->toPlainText();

    //发送数据包序列号
    int sequIn = 1+48 ;
    char sequCh[1];
    sequCh[0] = (char)sequIn;
    QByteArray sequBa = QByteArray(sequCh,1);

    //将short转角转为2字节的char数组
    short swSeSh = strSendSw.toShort();
    char swSeCh[2];
    send_short_char(swSeSh,swSeCh);
    QByteArray swSeBa = QByteArray(swSeCh,2);

//    //将float速度转为4字节的char数组
//    float vecSeFl = strSendVec.toFloat();
//    char vecSeCh[4];
//    send_float_char(vecSeFl,vecSeCh);
//    QByteArray vecSeBa = QByteArray(vecSeCh,4);

    //将unsigned short速度转为2节的char数组
    unsigned short velSeUS = strSendVec.toUShort();
    char velSeCh[2];
    send_ushort_char(velSeUS,velSeCh);
    QByteArray vecSeBa = QByteArray(velSeCh,2);

    //获取当前时间，包括秒与毫秒
    QTime current_time_start = QTime::currentTime();

    int secondStart = current_time_start.second();
    char secondStartChar[4];
    send_int_char(secondStart,secondStartChar);
    QByteArray secondStartBa = QByteArray(secondStartChar,4);

    int msecStart = current_time_start.msec();
    char msecStartChar[4];
    send_int_char(msecStart,msecStartChar);
    QByteArray msecStartBa = QByteArray(msecStartChar,4);

    //将1字节起始符、转角2字节、速度4字节写入发送ByteArray字节串
    QByteArray sendData;
    sendData.append("#");
    sendData.append(sequBa);
    sendData.append(swSeBa);
    sendData.append(vecSeBa);
    sendData.append(secondStartBa);
    sendData.append(msecStartBa);

    //将ByteArray数据写入串口发送
    serial.write(sendData);

    //获得发送串口的ByteArray数据长度，存储为int，以字符串形式显示
    int sendLengthNum = sendData.size();
//    int sendLengthNum = sizeof (inSData);
    QString sendLength = QString::number(sendLengthNum);
    ui->sendTextLength->append(sendLength);
}

//点击清除接收区显示数据
void MainWindow::on_clearButton_clicked()
{
    ui->recvTextEdit->clear();
}

//发送数据float转char函数
void MainWindow::send_float_char(float data, char *s)
{
    sendUnion.ff = data;
    for(int i=0;i<4;i++)
    {
        s[i] = sendUnion.ch[i];
    }
}

//发送数据int转char函数
void MainWindow::send_int_char(int data, char *s)
{
    sendUnion.ii = data;
    for(int i=0;i<4;i++)
    {
        s[i] = sendUnion.ch[i];
    }
}

//发送数据unsigned short转char函数
void MainWindow::send_ushort_char(unsigned short data, char *s)
{
    sendUnion.us = data;
    for(int i=0;i<2;i++)
    {
        s[i] = sendUnion.ch[i];
    }
}

//发送数据short转char函数
void MainWindow::send_short_char(short data, char *s)
{
    sendUnion.ss = data;
    for(int i=0;i<2;i++)
    {
        s[i] = sendUnion.ch[i];
    }
}


//接收数据char转float函数
void MainWindow::recv_float_char(float &data, char *s)
{
    for(int i=0;i<4;i++)
    {
        recvUnion.ch[i] = s[i];
    }
    data = recvUnion.ff;
}

//接收数据char转int函数
void MainWindow::recv_int_char(int &data, char *s)
{
    for(int i=0;i<4;i++)
    {
        recvUnion.ch[i] = s[i];
    }
    data = recvUnion.ii;
}

//接收数据char转unsigned short函数
void MainWindow::recv_ushort_char(unsigned short &data, char *s)
{
    for(int i=0;i<2;i++)
    {
        recvUnion.ch[i] = s[i];
    }
    data = recvUnion.us;
}

//接收数据char转short函数
void MainWindow::recv_short_char(short &data, char *s)
{
    for(int i=0;i<2;i++)
    {
        recvUnion.ch[i] = s[i];
    }
    data = recvUnion.ss;
}
