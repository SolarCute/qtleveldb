
#include "global.h"
#include <QtCore>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJSValue>

QString variantToJson(const QVariant variant)
{
    QString result = QJsonDocument(variantToJsonObject(variant)).toJson(QJsonDocument::Compact);
    return result;
}

QJsonObject variantToJsonObject(QVariant variant)
{
    QJsonObject doc;
    QVariant object = variant;

    //convert from JSValue to QVariantList or QVarianMap
    if (object.userType() == qMetaTypeId<QJSValue>()){
        QJSValue value = variant.value<QJSValue>();
        object = value.toVariant();
    }

    switch (object.type()) {
    case QVariant::Invalid:
        doc.insert("type", "invalid");
        break;
    case QVariant::String:
    case QVariant::LongLong:
    case QVariant::ULongLong:
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::Bool:
    case QVariant::Double:
    case QVariant::Char: {
        doc.insert("type", "primitive");
        doc.insert("data", QJsonValue::fromVariant(object));
        break;
    }
    case QVariant::ByteArray: {
        QByteArray a = object.toByteArray();
        doc.insert("type", "bytearray");
        doc.insert("data", QJsonValue(QString(a)));
        break;
    }
    case QVariant::KeySequence:
        doc.insert("type", "keysequence");
        doc.insert("data", object.toString());
        break;
    case QVariant::Rect: {
        QJsonObject sub;
        QRect r = qvariant_cast<QRect>(object);
        sub.insert("x", r.x());
        sub.insert("y", r.y());
        sub.insert("width", r.width());
        sub.insert("height", r.height());
        doc.insert("type", "rect");
        doc.insert("data", sub);
        break;
    }
    case QVariant::RectF: {
        QRectF r = qvariant_cast<QRectF>(object);
        QJsonObject sub;
        sub.insert("x", r.x());
        sub.insert("y", r.y());
        sub.insert("width", r.width());
        sub.insert("height", r.height());
        doc.insert("type", "rectf");
        doc.insert("data",sub);
        break;
    }
    case QVariant::Size: {
        QSize s = qvariant_cast<QSize>(object);
        QJsonObject sub;
        sub.insert("width", s.width());
        sub.insert("height", s.height());
        doc.insert("type", "size");
        doc.insert("data", sub);
        break;
    }
    case QVariant::SizeF: {
        QSizeF s = qvariant_cast<QSizeF>(object);
        QJsonObject sub;
        sub.insert("width", s.width());
        sub.insert("height", s.height());
        doc.insert("type", "sizef");
        break;
    }
    case QVariant::Point: {
        QPoint p = qvariant_cast<QPoint>(object);
        QJsonObject sub;
        sub.insert("x", p.x());
        sub.insert("y", p.y());
        doc.insert("type", "point");
        doc.insert("data", sub);
        break;
    }
    case QVariant::PointF: {
        QPointF p = qvariant_cast<QPointF>(object);
        QJsonObject sub;
        sub.insert("x", p.x());
        sub.insert("y", p.y());
        doc.insert("type", "pointf");
        doc.insert("data", sub);
        break;
    }
    case QMetaType::QDate: {
        QDate date = qvariant_cast<QDate>(object);
        QJsonObject sub;
        sub.insert("day", date.day());
        sub.insert("month", date.month());
        sub.insert("year", date.year());
        doc.insert("type", "date");
        doc.insert("data", sub);
        break;
    }
    case QMetaType::QTime: {
        QTime time = qvariant_cast<QTime>(object);
        QJsonObject sub;
        sub.insert("hour", time.hour());
        sub.insert("minute", time.minute());
        sub.insert("second", time.second());
        sub.insert("msec", time.msec());
        doc.insert("type", "time");
        doc.insert("data", sub);
        break;
    }
    case QMetaType::QDateTime:{
        QDateTime datetime = qvariant_cast<QDateTime>(object);
        doc.insert("type", "datetime");
        doc.insert("data", datetime.toString("yyyy-MM-ddTHH:mm:ss.zzzTZD"));
        break;
    }
    case QMetaType::QVariantMap: {
        QVariantMap map = object.toMap();
        QJsonObject sub;
        for(auto key : map.keys()){
            sub.insert(key, variantToJsonObject(map[key]));
        }
        doc.insert("type", "object");
        doc.insert("data", sub);
        break;
    }
    case QMetaType::QVariantList: {
        QVariantList list = object.toList();
        QJsonArray ary;
        for(auto elem : list){
            ary.append(variantToJsonObject(elem));
        }
        doc.insert("type", "list");
        doc.insert("data", ary);
        break;
    }

    default: {
#ifndef QT_NO_DATASTREAM
        QByteArray a;
        {
            QDataStream s(&a, QIODevice::WriteOnly);
            s.setVersion(QDataStream::Qt_4_9);
            s << object;
        }
        doc.insert("type", "variant");
        doc.insert("data", QString::fromLatin1(a.constData(), a.size()));
#else
        Q_ASSERT(!"QSettings: Cannot save custom types without QDataStream support");
#endif
        break;
    }
    }

    return doc;
}

QVariant jsonToVariant(QString json)
{
    QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject object = document.object();
    return jsonToVariant(object);
}


QVariant jsonToVariant(QJsonObject object)
{
    if (object.isEmpty()){
        return QVariant(QJsonValue(QJsonValue::Undefined));
    }
    if (object.value("type") == QString("primitive"))
        return object.value("data");
    else if (object.value("type") == QString("point")){
        QJsonObject data = object.value("data").toObject();
        return QVariant(QPoint(data.value("x").toInt(), data.value("y").toInt()));
    }
    else if (object.value("type") == QString("pointf")){
        QJsonObject data = object.value("data").toObject();
        return QVariant(QPoint(data.value("x").toDouble(), data.value("y").toDouble()));
    }
    else if (object.value("type") == QString("rect")){
        QJsonObject data = object.value("data").toObject();
        return QVariant(QRect(data.value("x").toInt(),
                              data.value("y").toInt(),
                              data.value("width").toInt(),
                              data.value("height").toInt()));
    }
    else if (object.value("type") == QString("rectf")){
        QJsonObject data = object.value("data").toObject();
        return QVariant(QRectF(data.value("x").toDouble(),
                               data.value("y").toDouble(),
                               data.value("width").toDouble(),
                               data.value("height").toDouble()));
    }
    else if (object.value("type") == QString("size")){
        QJsonObject data = object.value("data").toObject();
        return QVariant(QSize(data.value("width").toInt(), data.value("height").toInt()));
    }
    else if (object.value("type") == QString("sizef")){
        QJsonObject data = object.value("data").toObject();
        return QVariant(QSizeF(data.value("width").toDouble(), data.value("height").toDouble()));
    }
    else if (object.value("type") == QString("bytearray")){
        QVariant data = object.value("data").toVariant();
        return QVariant(QByteArray(data.value<QByteArray>()));
    }
    else if (object.value("type") == QString("keysequence")){
        return object.value("data").toString();
    }
    else if (object.value("type") == QString("date")){
        QJsonObject data = object.value("data").toObject();
        return QVariant(QDate(data.value("year").toInt(),
                              data.value("month").toInt(),
                              data.value("day").toInt()));
    }
    else if (object.value("type") == QString("time")){
        QJsonObject data = object.value("data").toObject();
        return QVariant(QTime(data.value("hour").toInt(),
                              data.value("minute").toInt(),
                              data.value("second").toInt(),
                              data.value("msec").toInt()));
    }
    else if (object.value("type") == QString("datetime")){
        QString data = object.value("data").toString();
        return QVariant(QDateTime::fromString(data, "yyyy-MM-ddTHH:mm:ss.zzzTZD"));
    }
    else if (object.value("type") == QString("list")){
        QJsonArray ary = object.value("data").toArray();
        QVariantList list;
        for(auto obj : ary){
            list.append(jsonToVariant(obj.toObject()));
        }
        return list;
    }
    else if (object.value("type") == QString("object")){
        QJsonObject data = object.value("data").toObject();
        QVariantMap map;
        for(auto key : data.keys()){
            map.insert(key, jsonToVariant(data[key].toObject()));
        }
        return map;
    }
    else if (object.value("type") == QString("variant")){
#ifndef QT_NO_DATASTREAM
        QString data = object.value("data").toString();
        QByteArray a(data.toLatin1().mid(9));
        QDataStream stream(&a, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_9);
        QVariant result;
        stream >> result;
        return result;
#else
        Q_ASSERT(!"QSettings: Cannot load custom types without QDataStream support");
#endif
    }
    return QVariant();
}
