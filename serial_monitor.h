#pragma once

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class SerialMonitor; }
QT_END_NAMESPACE

class SerialMonitor : public QMainWindow {
    Q_OBJECT

public:
    SerialMonitor(QWidget *parent = nullptr);
    ~SerialMonitor();

private slots:
    void fillPortList();
    void fillBaudRates();
    void connectToDevice();
    void disconnectFromDevice();
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);
    void checkDeviceConnection();
    void onDataReceived();

private:
    Ui::SerialMonitor *ui;
    QSerialPort *serialPort_;
    QTimer *idleTimer_;
    QComboBox *portComboBox_;
    QComboBox *baudRateComboBox_;
    QPushButton *connectButton_;
    QPushButton *disconnectButton_;
    QLabel *deviceStatusLabel_;
    QLabel *dataTransferStatusLabel_;
};
