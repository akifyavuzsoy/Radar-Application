#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDebug>
#include <QMessageBox>
#include <QtWidgets>
#include <QtGui>
#include <QtMath>
#include <QTimer>

#include "serialconnectdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Dialog;
}
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();


private slots:
    void on_btnConnect_clicked();

private:
    Ui::Dialog *ui;
    QGraphicsScene *scene;
    QPixmap pix;
    QGraphicsItem *rect;

    SerialConnectDialog *_serialDialog;

    float currAngle;
    const float r;
    const float angleOffset;
    float t_up;
    float t_lo;
    QPolygonF triangle;
    QGraphicsPolygonItem* needle;

    QByteArray SerialDatas;

    uint8_t deviceMajorVersion;
    uint8_t deviceMinorVersion;
    uint16_t servoPosition;
    float Distance;

    void parseDataForRadarApp();

};
#endif // DIALOG_H
