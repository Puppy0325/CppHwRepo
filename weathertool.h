#ifndef WEATHERTOOL_H
#define WEATHERTOOL_H
#include<QMap>
#include<QFile>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include<QJsonParseError>
#include<QJsonValue>
class WeatherTool{
private:
    static QMap<QString,QString> mCityMap;
    static void initCityMap(){
        QString filepath="D:/citycode.json";

        QFile file(filepath);
        file.open(QIODevice::ReadOnly|QIODevice::Text);
        QByteArray json=file.readAll();
        file.close();

        QJsonParseError err;
        QJsonDocument doc=QJsonDocument::fromJson(json,&err);
        if(err.error!= QJsonParseError::NoError){
            return;
        }
        if(!doc.isArray()){
            return;
        }
        QJsonArray cities=doc.array();
        for(int i=0;i<cities.size();i++){
            QString city=cities[i].toObject().value("city_name").toString();
            QString code=cities[i].toObject().value("city_code").toString();
            mCityMap.insert(city,code);
        }
        QMapIterator<QString, QString> i(mCityMap);
        // while (i.hasNext()) {
        //     i.next();
        //     qDebug() << i.key() << i.value();
        // }

    }
public:
    static QString getCityCode(QString cityName){
        if(mCityMap.isEmpty()){
            initCityMap();
        }
        QMap<QString,QString>::iterator it=mCityMap.find(cityName);
        if(it==mCityMap.end()){
            it=mCityMap.find(cityName+"市");
        }
        if(it!=mCityMap.end()){
            return it.value();
        }
        return "";
    };
    QMap<QString, QString> getMap() {
        return mCityMap;
    }
    ~WeatherTool() {};
};
QMap<QString,QString> WeatherTool::mCityMap={};
#endif // WEATHERTOOL_H
