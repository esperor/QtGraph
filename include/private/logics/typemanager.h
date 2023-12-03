#pragma once

#include <QObject>

#include <QJsonObject>
#include <QMap>

#include "qtgraph.h"

#include "typemanager.pb.h"

namespace qtgraph {

/*
    Here i use polymorphism instead of templated class for one
    reason: qobject (and thus qt life-cycle) doesn't mix with templates

*/

class TypeManager : public QObject
{
    Q_OBJECT

public:
    TypeManager() {}
    TypeManager(const TypeManager &other);

    const QVector<QJsonObject> &Types() const { return _types; }
    const QMap<QString, int> &TypeNames() const { return _typeNames; }

    inline QString typeNameByID(int id) const { return Types()[id].value("name").toString(); }

    bool readTypes(const char *file);
    inline bool readTypes(const QString &file) { return readTypes(file.toStdString().c_str()); }
    inline bool readTypes(const std::string &file) { return readTypes(file.c_str()); }

    virtual QString keyValue() const = 0;

    void setFileName(QString name) { _filename = name; }
    const QString &getFileName() const { return _filename; }

    protocol::TypeManager toProtocolTypeManager() const;

    bool operator!=(const TypeManager &other);
    void operator=(const TypeManager &other);

protected:
    QVector<QJsonObject> _types = {};
    QMap<QString, int> _typeNames = {};
    QString _filename = "";

};

class PinTypeManager final : public TypeManager
{
public:
    PinTypeManager() {}

    QString keyValue() const override { return "pins"; }

    static PinTypeManager *fromProtocolTypeManager(const protocol::TypeManager &tm);
};

class NodeTypeManager final : public TypeManager
{
public:
    NodeTypeManager() {}

    QString keyValue() const override { return "nodes"; }

    static NodeTypeManager *fromProtocolTypeManager(const protocol::TypeManager &tm);
};

}
