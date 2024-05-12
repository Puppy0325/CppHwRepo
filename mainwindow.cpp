#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QContextMenuEvent>
#include<QString>
#include<QDebug>
#include<QMenu>
#include<QMessageBox>
#include<QNetworkAccessManager>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);//设置窗口无边框
    setFixedSize(width(),height());//设置固定窗口大小

    //构建右键菜单
    mExitMenu=new QMenu(this);//右键退出的菜单
    mExitAct=new QAction();
    mExitAct->setText("退出");
   // mExitAct->setIcon(QIcon(" "));//添加图标
    mExitMenu->addAction(mExitAct);

    connect(mExitAct,&QAction::triggered,this,[=]{
      qApp->exit(0);
    });

    mWeekList<<ui->lblweek0<<ui->lblweek1<<ui->lblweek2<<ui->lblweek3<<ui->lblweek4<<ui->lblweek5;
    mDateList<<ui->lbldate0<<ui->lbldate1<<ui->lbldate2<<ui->lbldate3<<ui->lbldate4<<ui->lbldate5;
    mTypeList<<ui->lbltype0<<ui->lbltype1<<ui->lbltype2<<ui->lbltype3<<ui->lbltype4<<ui->lbltype5;
    mTypeIconList<<ui->lblicon0<<ui->lblicon1<<ui->lblicon2<<ui->lblicon3<<ui->lblicon4<<ui->lblicon5;
    mAqiList<<ui->lblaqi0<<ui->lblaqi1<<ui->lblaqi2<<ui->lblaqi3<<ui->lblaqi4<<ui->lblaqi5;
    mfxList<<ui->lblfx0<<ui->lblfx1<<ui->lblfx2<<ui->lblfx3<<ui->lblfx4<<ui->lblfx5;
    mflList<<ui->lblfl0<<ui->lblfl1<<ui->lblfl2<<ui->lblfl3<<ui->lblfl4<<ui->lblfl5;

    // mTypeMap.insert("暴雪","");
    mNetAccessManager=new QNetworkAccessManager(this);
    connect(mNetAccessManager,&QNetworkAccessManager::finished,this,&MainWindow::onReplied);
    
    //直接在构造中，请求天气数据
    getWeatherInfo("101010100");
}

MainWindow::~MainWindow(){delete ui;}
//重写父类的虚函数
void MainWindow::contextMenuEvent(QContextMenuEvent *event){
    mExitMenu->exec(QCursor::pos());
    event->accept();
}
void MainWindow::mousePressEvent(QMouseEvent *event){
    mOffset= event->globalPos() - this->pos();
}
void MainWindow::mouseMoveEvent(QMouseEvent *event){
    this->move(event->globalPos()-mOffset);
}
void MainWindow:: getWeatherInfo(QString cityCode){
    QUrl url("http://t.weather.itboy.net/api/weather/city/"+cityCode);
    mNetAccessManager->get(QNetworkRequest(url));
}

void MainWindow:: parseJson(QByteArray &byteArray){
    QJsonParseError err;
    QJsonDocument doc=QJsonDocument::fromJson(byteArray,&err);
    if(err.error!=QJsonParseError::NoError) {
        return;
    }

    QJsonObject rootObj=doc.object();
    //qDebug()<<rootObj.value("message").toString();

    //解析日期和城市
    mToday.date=rootObj.value("date").toString();
    mToday.city=rootObj.value("cityInfo").toObject().value("city").toString();

    //解析yesterday
    QJsonObject objData=rootObj.value("data").toObject();
    QJsonObject objYesterday=objData.value("yesterday").toObject();
    mDay[0].week=objYesterday.value("week").toString();
    mDay[0].date=objYesterday.value("ymd").toString();
    mDay[0].type=objYesterday.value("type").toString();

    //获取高温和低温
    QString s;
    s=objYesterday.value("high").toString().split(" ").at(1);
    mDay[0].high= s.left(s.length()-1).toInt();

    s=objYesterday.value("low").toString().split(" ").at(1);
    mDay[0].low=s.left(s.length()-1).toInt();

//解析风向和风力
    mDay[0].fx=objYesterday.value("fx").toString( );
    mDay[0].fl=objYesterday.value("fl").toString( );
//污染指数
    mDay[0].aqi=objYesterday.value("aqi").toDouble( );

    //解析forecast中五天的数据
    QJsonArray forecastArr=objData.value("forecast").toArray();

    for(int i=0;i<5;i++){
        QJsonObject objForecast= forecastArr[i].toObject();
        mDay[i+1].week=objForecast.value("week").toString();
        mDay[i+1].date=objForecast.value("ymd").toString();
        mDay[i+1].type =objForecast.value("type").toString();

        QString s;
        s=objForecast.value("high").toString().split(" ").at(1);
        mDay[i+1].high=s.left(s.length()-1).toInt();

        s=objForecast.value("low").toString().split(" ").at(1);
        mDay[i+1].low=s.left(s.length()-1).toInt();

        mDay[i+1].fx=objForecast.value("fx").toString( );
        mDay[i+1].fl=objForecast.value("fl").toString( );

        mDay[i+1].aqi=objForecast.value("aqi").toDouble( );
    }

    //解析今天的数据
    mToday.ganmao=objData.value("ganmao").toString();
    mToday.wendu=objData.value("wendu").toString();
    mToday.shidu=objData.value("shidu").toString();
    mToday.pm25=objData.value("pm25").toDouble();
    mToday.quality=objData.value("quality").toString();

    mToday.type=mDay[1].type;

    mToday.fx=mDay[1].fx;
    mToday.fl=mDay[1].fl;

    mToday.high=mDay[1].high;
    mToday.low=mDay[1].low;

       updateUi();
}
void MainWindow::updateUi(){
    ui->lblDate->setText(QDateTime::fromString(mToday.date,"yyyyMMdd").toString("yyyy/MM/dd")+" "+mDay[1].week);
    ui->lblcity->setText(mToday.city);

    //ui->lblTypeIcon->setPixmap
    ui->lbltemp->setText(mToday.wendu);
    ui->lbltype->setText(mToday.type);
    // ui->lbllowhigh->setText(QString::number(mToday.low)+"~"+QString::number(mToday.high)+"°C");
    QString range = "°C";
    range = QString::asprintf("%d ~ %d°C", mToday.low, mToday.high);
    ui->lbllowhigh->setText(range);
    ui->lblganmao->setText("感冒指数:"+mToday.ganmao);
    ui->lblFx->setText(mToday.fx);
    ui->lblFl->setText(mToday.fl);

    ui->lblpm25->setText(QString::number(mToday.pm25));

    ui->lblshidu->setText(mToday.shidu);
    ui->lblquality->setText(mToday.quality);

    for(int i=0;i<6;i++){
        mWeekList[i]->setText("周"+mDay[i].week.right(1));
        ui->lblweek0->setText("昨天");
         ui->lblweek1->setText("今天");
         ui->lblweek2->setText("明天");

         QStringList ymdList=mDay[i].date.split("-");
         mDateList[i]->setText(ymdList[1]+"/"+ymdList[2]);

         mTypeList[i]->setText(mDay[i].type);

         if(mDay[i].aqi>=0&&mDay[i].aqi<=50){
             mAqiList[i]->setText("优");
             mAqiList[i]->setStyleSheet("background-color:rgb(121,184,0);");
         }else  if(mDay[i].aqi>50&&mDay[i].aqi<=100){
             mAqiList[i]->setText("良");
         mAqiList[i]->setStyleSheet("background-color:rgb(255,187,23);");
             }else  if(mDay[i].aqi>100&&mDay[i].aqi<=150){
         mAqiList[i]->setText("轻度");
         mAqiList[i]->setStyleSheet("background-color:rgb(255,87,97);");
         }else  if(mDay[i].aqi>150&&mDay[i].aqi<=200){
         mAqiList[i]->setText("中度");
         mAqiList[i]->setStyleSheet("background-color:rgb(235,17,27);");
         }else  if(mDay[i].aqi>200&&mDay[i].aqi<=250){
         mAqiList[i]->setText("重度");
         mAqiList[i]->setStyleSheet("background-color:rgb(170,0,0);");
         }else {
             mAqiList[i]->setText("严重");
             mAqiList[i]->setStyleSheet("background-color:rgb(110,0,0);");
         }
         mfxList[i]->setText(mDay[i].fx);
         mflList[i]->setText(mDay[i].fl);
    }

}
void MainWindow::onReplied(QNetworkReply* reply){
    int status_code= reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug()<<"operation"<<reply->operation();
    //qDebug()<<"status code"<<status_code;
   // qDebug()<<"url"<<reply->url();
    //qDebug()<<"raw header"<<reply->rawHeaderList();
    if(reply->error()!=QNetworkReply::NoError||status_code!=200){
        qDebug()<<reply->errorString().toLatin1().data();
        QMessageBox::warning(this,"天气","请求数据失败",QMessageBox::Ok);
    }else{
        QByteArray byteArray=reply->readAll();
        // qDebug()<<"read all"<<byteArray.data();
        parseJson(byteArray);
    }
    reply->deleteLater();
}
