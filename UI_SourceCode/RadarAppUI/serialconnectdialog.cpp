#include "serialconnectdialog.h"
#include "ui_serialconnectdialog.h"

SerialConnectDialog::SerialConnectDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SerialConnectDialog)
    , _serialPort(nullptr)
{
    ui->setupUi(this);


    loadPorts();

    ui->cmbBaudrates->setCurrentText("115200");
    ui->cmbParitys->setCurrentText("None");
    ui->cmbDataBits->setCurrentText("8");
    ui->cmbStopBits->setCurrentText("1");
    ui->cmbParitysError->setCurrentText("63 ('?')");
}

SerialConnectDialog::~SerialConnectDialog()
{
    delete ui;
}

void SerialConnectDialog::on_btnPortsInfo_clicked()
{
    ui->cmbPorts->clear();
    foreach (auto &port, QSerialPortInfo::availablePorts()) {
        ui->cmbPorts->addItem(port.portName());
    }
}

void SerialConnectDialog::loadPorts()
{
    foreach (auto &port, QSerialPortInfo::availablePorts()) {
        ui->cmbPorts->addItem(port.portName());
    }
}

void SerialConnectDialog::on_btnOpenPort_clicked()
{
    if (_serialPort != nullptr) {
        _serialPort->close();
        delete _serialPort;
    }

    _serialPort = new QSerialPort(this);
    _serialPort->setPortName(ui->cmbPorts->currentText());
    //_serialPort->setBaudRate(QSerialPort::Baud115200);
    //_serialPort->setDataBits(QSerialPort::Data8);
    //_serialPort->setParity(QSerialPort::NoParity);
    //_serialPort->setStopBits(QSerialPort::OneStop);

    // Set Baud Rate Value:
    _serialPort->setBaudRate(ui->cmbBaudrates->currentText().toInt());
    // Set Data Bit Value:
    if(ui->cmbDataBits->currentText().toInt() == QSerialPort::Data5)
        _serialPort->setDataBits(QSerialPort::Data5);
    else if (ui->cmbDataBits->currentText().toInt() == QSerialPort::Data6)
        _serialPort->setDataBits(QSerialPort::Data6);
    else if (ui->cmbDataBits->currentText().toInt() == QSerialPort::Data7)
        _serialPort->setDataBits(QSerialPort::Data7);
    else
        _serialPort->setDataBits(QSerialPort::Data8);
    // Set Parity Value:
    if(ui->cmbParitys->currentIndex() == 0)
        _serialPort->setParity(QSerialPort::NoParity);
    else if(ui->cmbParitys->currentIndex() == 1)
        _serialPort->setParity(QSerialPort::EvenParity);
    else if(ui->cmbParitys->currentIndex() == 2)
        _serialPort->setParity(QSerialPort::OddParity);
    else if(ui->cmbParitys->currentIndex() == 3)
        _serialPort->setParity(QSerialPort::SpaceParity);
    else if(ui->cmbParitys->currentIndex() == 4)
        _serialPort->setParity(QSerialPort::MarkParity);
    // Set Stop Bit Value:
    if(ui->cmbStopBits->currentIndex() == 0)
        _serialPort->setStopBits(QSerialPort::OneStop);
    else if(ui->cmbStopBits->currentIndex() == 1)
        _serialPort->setStopBits(QSerialPort::OneAndHalfStop);
    else if(ui->cmbStopBits->currentIndex() == 2)
        _serialPort->setStopBits(QSerialPort::TwoStop);


    if (_serialPort->open(QIODevice::ReadWrite)) {
        //QMessageBox::information(this, "Result", "Port opened successfully...");
        connect(_serialPort, &QSerialPort::readyRead, this, &SerialConnectDialog::readData);
        _timerSerialData->start(500);

    }
    else {
        QMessageBox::critical(this, "Port Error", "Unable to open specified port!!!");
    }

    // TODO: Open olduğunda messagebox gelmesin onun yerine dialogra ikon yeşil ve bağlı olduğu gösterilsin
    // TODO: Close seçeneğide olsun close olduğunda dialogtaki ikon kırmızı olsun
}

void SerialConnectDialog::readData()
{
    if(!_serialPort->isOpen()) {
        QMessageBox::critical(this, "Port Error", "Port is not opened!!!");
        return;
    }
    //auto data = _serialPort->readAll();
    tmp = _serialPort->read(128);

    if(tmp.size() > 0 && CRC_check(tmp)) {
        _serialDatas = tmp;
        QString hexData = _serialDatas.toHex();
        //qDebug() << hexData;
        tmp.clear();
    }

    _isConnected = true;

}

bool SerialConnectDialog::CRC_check(const QByteArray& message)
{
    unsigned int CRCFull = 0xFFFF;
    unsigned int CRCLSB;

    // Verinin uzunluğunu hesapla (Son 2 byte CRC olduğu için -2 yapıyoruz)
    int message_length = message.size() - 2;

    // CRC hesaplama döngüsü
    for (int i = 0; i < message_length; i++) {
        CRCFull = static_cast<unsigned int>(CRCFull ^ static_cast<unsigned char>(message[i]));

        for (int j = 0; j < 8; j++) {
            CRCLSB = static_cast<unsigned int>(CRCFull & 0x0001);
            CRCFull = static_cast<unsigned int>((CRCFull >> 1) & 0x7FFF);

            if (CRCLSB == 1) {
                CRCFull = static_cast<unsigned int>(CRCFull ^ 0xA001);
            }
        }
    }

    // CRC sonuçlarını ayır
    unsigned char CRCHigh = static_cast<unsigned char>((CRCFull >> 8) & 0xFF);
    unsigned char CRCLow  = static_cast<unsigned char>(CRCFull & 0xFF);

    // CRC'yi kontrol et (Verinin son iki byte'ı ile hesaplanan CRC'nin eşleşip eşleşmediğini kontrol ediyoruz)
    return (CRCLow == static_cast<unsigned char>(message[message_length]) &&
            CRCHigh == static_cast<unsigned char>(message[message_length + 1]));
}


void SerialConnectDialog::GetCRC(QByteArray& message)
{
    unsigned int CRCFull = 0xFFFF;
    unsigned int CRCLSB;

    // Mesajın uzunluğu (Son iki byte CRC için ayrıldığından -2 yapıyoruz)
    int message_length = message.size() - 2;

    // CRC hesaplama döngüsü
    for (int i = 0; i < message_length; i++) {
        CRCFull = static_cast<unsigned int>(CRCFull ^ static_cast<unsigned char>(message[i]));

        for (int j = 0; j < 8; j++) {
            CRCLSB = static_cast<unsigned int>(CRCFull & 0x0001);
            CRCFull = static_cast<unsigned int>((CRCFull >> 1) & 0x7FFF);

            if (CRCLSB == 1) {
                CRCFull = static_cast<unsigned int>(CRCFull ^ 0xA001);
            }
        }
    }

    // CRC sonuçlarını ayır
    unsigned char CRCHigh = static_cast<unsigned char>((CRCFull >> 8) & 0xFF);
    unsigned char CRCLow  = static_cast<unsigned char>(CRCFull & 0xFF);

    // CRC değerlerini mesaja ekle
    message[message_length] = CRCLow;
    message[message_length + 1] = CRCHigh;
}

