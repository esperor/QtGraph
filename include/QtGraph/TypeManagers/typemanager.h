#pragma once

#include <QObject>

#include <QJsonObject>
#include <QMap>

#include "typemanager.pb.h"

namespace GraphLib {

class TypeManager : public QObject
{
    Q_OBJECT

public:
    TypeManager() {}
    TypeManager(const TypeManager &other);

    const QVector<QJsonObject> &Types() const { return _types; }
    const QMap<QString, int> &TypeNames() const { return _typeNames; }

    inline QString typeNameByID(int id) const { return Types()[id].value("name").toString(); }

    virtual bool readTypes(const char *file) = 0;

    void setFileName(QString name) { _filename = name; }
    const QString &getFileName() const { return _filename; }

    protocol::TypeManager toProtocolTypeManager();

    bool operator!=(const TypeManager &other);
    void operator=(const TypeManager &other);

protected:
    QVector<QJsonObject> _types = {};
    QMap<QString, int> _typeNames = {};
    QString _filename = "";

};

}
