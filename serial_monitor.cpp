#include "serial_monitor.h"
#include "ui_serial_monitor.h"
#include "version.h"

SerialMonitor::SerialMonitor(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::SerialMonitor),
serialPort_(new QSerialPort(this)) {
    ui->setupUi(this);

    QLabel *versionLabel = new QLabel(this);
    versionLabel->setAlignment(Qt::AlignRight);
    versionLabel->setText(tr("Версия: %1 Build: %2 %3").arg(VERSION).arg(BUILD_DATE).arg(BUILD_TIME));
    versionLabel->setStyleSheet("color: grey");

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(versionLabel);

    portComboBox_ = new QComboBox(this);
    baudRateComboBox_ = new QComboBox(this);
    layout->addWidget(new QLabel("Выбрать COM порт:", this));
    layout->addWidget(portComboBox_);
    layout->addWidget(new QLabel("Выбрать скорость (бит/с):", this));
    layout->addWidget(baudRateComboBox_);

    connectButton_ = new QPushButton("Подключиться", this);
    layout->addWidget(connectButton_);
    disconnectButton_ = new QPushButton("Отключиться", this);
    layout->addWidget(disconnectButton_);
    disconnectButton_->setEnabled(false);

    deviceStatusLabel_ = new QLabel("Статус устройства: Отключено", this);
    deviceStatusLabel_->setStyleSheet("color: red;");
    layout->addWidget(deviceStatusLabel_);
    dataTransferStatusLabel_ = new QLabel("Статус передачи данных: Ожидание", this);
    dataTransferStatusLabel_->setStyleSheet("color: orange;");
    layout->addWidget(dataTransferStatusLabel_);

    setCentralWidget(centralWidget);

    fillPortList();
    fillBaudRates();
    baudRateComboBox_->setCurrentIndex(3);

    idleTimer_ = new QTimer(this);
    idleTimer_->setInterval(1000);
    idleTimer_->setSingleShot(true);
    connect(idleTimer_, &QTimer::timeout, this, [this]() {
        dataTransferStatusLabel_->setText("Статус передачи данных: Ожидание");
        dataTransferStatusLabel_->setStyleSheet("color: orange;");
    });

    connect(connectButton_, &QPushButton::clicked, this, &SerialMonitor::connectToDevice);
    connect(disconnectButton_, &QPushButton::clicked, this, &SerialMonitor::disconnectFromDevice);
    connect(serialPort_, &QSerialPort::readyRead, this, &SerialMonitor::handleReadyRead);
    connect(serialPort_, &QSerialPort::errorOccurred, this, &SerialMonitor::handleError);
    connect(serialPort_, &QSerialPort::readyRead, this, &SerialMonitor::onDataReceived);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SerialMonitor::checkDeviceConnection);
    timer->start(1000);
}

SerialMonitor::~SerialMonitor() {
    delete ui;
}

void SerialMonitor::fillPortList() {
    portComboBox_->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        portComboBox_->addItem(info.portName());
    }
}

void SerialMonitor::fillBaudRates() {
    baudRateComboBox_->addItem("1200", QSerialPort::Baud1200);
    baudRateComboBox_->addItem("2400", QSerialPort::Baud2400);
    baudRateComboBox_->addItem("4800", QSerialPort::Baud4800);
    baudRateComboBox_->addItem("9600", QSerialPort::Baud9600);
    baudRateComboBox_->addItem("19200", QSerialPort::Baud19200);
    baudRateComboBox_->addItem("38400", QSerialPort::Baud38400);
}

void SerialMonitor::connectToDevice() {
    QString portName = portComboBox_->currentText();
    serialPort_->setPortName(portName);
    serialPort_->setBaudRate(baudRateComboBox_->currentData().toInt());

    if (serialPort_->open(QIODevice::ReadWrite)) {
        deviceStatusLabel_->setText("Статус устройства: Подключено");
        deviceStatusLabel_->setStyleSheet("color: green");
        connectButton_->setEnabled(false);
        disconnectButton_->setEnabled(true);
    } else {
        QMessageBox::critical(this, "Ошибка", "Невозможно открыть последовательный порт");
    }
}

void SerialMonitor::disconnectFromDevice() {
    serialPort_->close();
    deviceStatusLabel_->setText("Статус устройства: Отключено");
    deviceStatusLabel_->setStyleSheet("color: red;");
    connectButton_->setEnabled(true);
    disconnectButton_->setEnabled(false);
    dataTransferStatusLabel_->setText("Статус передачи данных: Ожидание");
    dataTransferStatusLabel_->setStyleSheet("color: orange;");
}

void SerialMonitor::handleReadyRead() {
    QByteArray data = serialPort_->readAll();
    dataTransferStatusLabel_->setText("Статус передачи данных: Чтение");
    dataTransferStatusLabel_->setStyleSheet("color: green;");
}

void SerialMonitor::handleError(QSerialPort::SerialPortError error) {
    if (error != QSerialPort::NoError) {
        QMessageBox::critical(this, "Ошибка", serialPort_->errorString());
        disconnectFromDevice();
    }
}

void SerialMonitor::checkDeviceConnection() {
    if (!serialPort_->isOpen() || !serialPort_->isWritable()) {
        deviceStatusLabel_->setText("Статус устройства: Отключено");
        deviceStatusLabel_->setStyleSheet("color: red;");
    }
}

void SerialMonitor::onDataReceived() {
    QByteArray data = serialPort_->readAll();

    dataTransferStatusLabel_->setText("Статус передачи данных: Чтение");
    dataTransferStatusLabel_->setStyleSheet("color: green;");

    idleTimer_->start();
}
