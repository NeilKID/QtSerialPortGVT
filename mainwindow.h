#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>        //提供访问串口的功能
#include <QSerialPortInfo>    //提供系统中存在的串口的信息

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSerialPort serial;
    //定义int、float与char数组共同体
    union myUnion
    {
        float ff;
        int ii;
        unsigned short us;
        short ss;
        char ch[4];
    }sendUnion,recvUnion;
    void send_float_char(float data, char *s);
    void send_ushort_char(unsigned short data, char *s);
    void send_short_char(short data, char *s);
    void recv_float_char(float &data, char *s);
    void recv_ushort_char(unsigned short &data, char *s);
    void recv_short_char(short &data, char *s);

private slots:
    void serialPort_readyRead();
    void on_searchButton_clicked();
    void on_openButton_clicked();
    void on_sendButton_clicked();
    void on_clearButton_clicked();

};
#endif // MAINWINDOW_H


