#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QLabel>
#include<QMouseEvent>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include"weatherdata.h"
// #include "weathertool.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QMenu*mExitMenu;//右键退出的菜单
    QAction*mExitAct;//退出的行为-菜单项
    QPoint mOffset;
    QNetworkAccessManager*mNetAccessManager;
    Today mToday;
    Day mDay[6];

    QList<QLabel*>mWeekList;
    QList<QLabel*>mDateList;

    QList<QLabel*>mTypeList;
    QList<QLabel*>mTypeIconList;

    QList<QLabel*>mAqiList;

    QList<QLabel*>mfxList;
    QList<QLabel*>mflList;

    QMap<QString, QString>mTypeMap;


protected:
     void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent *event);
     void mouseMoveEvent(QMouseEvent *event);
    void onReplied(QNetworkReply* reply);
     void getWeatherInfo(QString cityCode);
    void  parseJson(QByteArray& byteArray);
     void updateUi();
    bool eventFilter(QObject*watched,QEvent*event);
     void paintHighCurve();
    void paintLowCurve();
private slots:
    void on_pushButton_clicked();
};
#endif // MAINWINDOW_H
