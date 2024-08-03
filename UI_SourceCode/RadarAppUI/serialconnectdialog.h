#ifndef SERIALCONNECTDIALOG_H
#define SERIALCONNECTDIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>

namespace Ui {
class SerialConnectDialog;
}

class SerialConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialConnectDialog(QWidget *parent = nullptr);
    ~SerialConnectDialog();

    QByteArray _serialDatas;
    bool _isConnected;
    QTimer *_timerSerialData;

private slots:
    void on_btnPortsInfo_clicked();

    void on_btnOpenPort_clicked();

private:
    Ui::SerialConnectDialog *ui;

    QSerialPort *_serialPort;
    QByteArray tmp;

    void loadPorts();
    void readData();
    bool CRC_check(const QByteArray& message);
    void GetCRC(QByteArray& message);
};

#endif // SERIALCONNECTDIALOG_H
