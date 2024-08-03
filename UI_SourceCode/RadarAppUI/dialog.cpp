#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , r(445.0) //needle radius (pixels)
    , angleOffset(0.05) //needle angle offset (rad)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    _serialDialog = new SerialConnectDialog(this);
    _serialDialog->_isConnected = false;

    //load bg image
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    pix = QPixmap(":/src/img/radar.png");
    scene->addPixmap(pix);


    //initialize needle at 0 degrees
    QPen blackpen(Qt::black);
    QBrush graybrush(Qt::gray);
    t_up = angleOffset;
    t_lo = -angleOffset;
    triangle.append(QPointF(r*qCos(t_up) + 505, -r*qSin(t_up) + 495));
    triangle.append(QPointF(505,495));
    triangle.append(QPointF(r*qCos(t_lo) + 505, -r*qSin(t_lo) + 495));
    needle = scene->addPolygon(triangle,blackpen,graybrush);
    needle->setOpacity(0.30);

    _serialDialog->_timerSerialData = new QTimer(this); // QTimer nesnesini oluşturun
    connect(_serialDialog->_timerSerialData, &QTimer::timeout, this, &Dialog::parseDataForRadarApp);
}


Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_btnConnect_clicked()
{
    _serialDialog->show();
}

void Dialog::parseDataForRadarApp()
{
    QPen blackpen(Qt::black);
    QBrush graybrush(Qt::gray);

    if(_serialDialog->_isConnected) {
        _serialDialog->close();
        QString hexData = _serialDialog->_serialDatas.toHex();
        qDebug() << hexData;
        if(_serialDialog->_serialDatas.size() > 0) {
            SerialDatas = _serialDialog->_serialDatas;
            //_serialDialog->_serialDatas.clear();

            if((static_cast<unsigned char>(SerialDatas[0]) == 0xFA) && (static_cast<unsigned char>(SerialDatas[1]) == 0xFB) &&
                (static_cast<unsigned char>(SerialDatas[124]) == 0xFB) && (static_cast<unsigned char>(SerialDatas[125]) == 0xFA)) {
                deviceMajorVersion = static_cast<uint8_t>(SerialDatas[2]);
                deviceMinorVersion = static_cast<uint8_t>(SerialDatas[3]);
                servoPosition = ((static_cast<uint8_t>(SerialDatas[8])) << 8) | (static_cast<uint8_t>(SerialDatas[9]));
                Distance = ((static_cast<uint8_t>(SerialDatas[10])) << 24) | ((static_cast<uint8_t>(SerialDatas[11])) << 16) | ((static_cast<uint8_t>(SerialDatas[12])) << 8) | (static_cast<uint8_t>(SerialDatas[13]));

                currAngle = qDegreesToRadians(servoPosition);
                float dist = Distance;
                if (dist < 100) {
                    float radius = dist*4.5;
                    float x = radius*qCos(currAngle);
                    float y = radius*qSin(currAngle);
                    float xT = x + 505;
                    float yT = -1*y + 495;
                    rect = scene->addRect(xT,yT,12,12,blackpen,graybrush);
                    rect->setOpacity(0.75);
                }

                ui->txt_majorVersion->setText(QString::number(deviceMajorVersion));
                ui->txt_minorVersion->setText(QString::number(deviceMinorVersion));
                ui->txt_Angle->setText(QString::number(servoPosition));
                ui->txt_Range->setText(QString::number(Distance));

                SerialDatas.clear();
            }
        }
    }

}
