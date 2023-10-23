#include <gtest/gtest.h>

#include <QtCore/qglobal.h>
#include <QString>
#include <QByteArray>
#include <QMap>
#include <QJsonObject>
#include <QPointF>
#include <algorithm>
#include <string>
#include <QSet>

#include <QtGraph/NodeFactory>
#include <QtGraph/TypeManagers>
#include "private/models/nodespawndata.h"
#include "private/utilities/utility.h"
#include "private/helpers/idgenerator.h"
#include "private/models/pindata.h"

using namespace testing;
using namespace qtgraph;

class TypeManagers : public ::testing::Test
{
protected:
    TypeManagers() {}

    void SetUp() override
    {
        _NodeTypeManager = NodeTypeManager();
        _PinTypeManager =  PinTypeManager();

        QString path = "test_files/";
        QString filename = "types.json";
        ASSERT_TRUE(_NodeTypeManager.readTypes(path + filename));
        ASSERT_TRUE(_PinTypeManager.readTypes(path + filename));
    }

    NodeTypeManager _NodeTypeManager;
    PinTypeManager _PinTypeManager;
};



TEST(IPinData, ByteArrayConversions)
{
    IPinData first(EPinDirection::In, 5, 6);
    QByteArray arr = first.toByteArray();
    IPinData second = IPinData::fromByteArray(arr);
    EXPECT_EQ(first, second);

    first.nodeID = 0;
    arr = first.toByteArray();
    second = IPinData::fromByteArray(arr);
    EXPECT_EQ(first, second);
}

TEST(INodeSpawnData, ByteArrayConversions)
{
    QString str("Text");
    INodeSpawnData first(str);
    QByteArray arr1 = first.toByteArray();
    INodeSpawnData second = INodeSpawnData::fromByteArray(arr1);
    EXPECT_EQ(first, second);

    ITypedNodeSpawnData third(str, 1);
    QByteArray arr2 = third.toByteArray();
    ITypedNodeSpawnData fourth = ITypedNodeSpawnData::fromByteArray(arr2);
    EXPECT_EQ(third, fourth);
}

TEST(Utilities, Snapping)
{
    auto pointToString = [](QPoint point) {
        return std::to_string(point.x()) + ", " + std::to_string(point.y());
    };

    QPointF point1(167.4f, 129.0f);
    QPointF point2(10.2f, 14.1f);

    QVector< std::pair<QPoint, QPoint> > assertions =
        {
            { QPoint(170, 130), snap(point1, 10) },
            { QPoint(165, 130), snap(point1, 5) },
            { QPoint(10, 15), snap(point2, 5) },
            { QPoint(10, 14), snap(point2, 1) },
        };

    auto lambda = [&](const QPoint &correct, const QPoint &result) {
        EXPECT_EQ(correct, result) << "Expected " << pointToString(correct) << " and got " << pointToString(result);
    };

    std::ranges::for_each(assertions, [&](const std::pair<QPoint, QPoint> &pair){
        lambda(pair.first, pair.second);
    });
}

TEST(Utilities, ParseToColor)
{
    auto check = [](QString str, int r, int g, int b) {
        QColor clr = parseToColor(str);

        EXPECT_EQ(clr.isValid(), true);

        EXPECT_EQ(clr.red(), r);
        EXPECT_EQ(clr.green(), g);
        EXPECT_EQ(clr.blue(), b);
    };

    QString str1("00AF00");
    check(str1, 0, 0xAF, 0);
    QString str2("2F03DD");
    check(str2, 0x2F, 0x03, 0xDD);
    QString str3("FFFFFF");
    check(str3, 0xFF, 0xFF, 0xFF);
}

TEST(IDGenerator, General)
{
    IDGenerator gen;
    EXPECT_EQ(0, gen.generate());
    EXPECT_EQ(1, gen.generate());
    EXPECT_EQ(2, gen.generate());
    gen.removeTaken(1);
    EXPECT_EQ(1, gen.generate());
    EXPECT_EQ(3, gen.generate());
    gen = IDGenerator(std::set({ 0U, 1U, 2U, 3U, 4U, 5U, 6U }));
    EXPECT_EQ(7, gen.generate());
    gen.removeTaken(4);
    EXPECT_EQ(4, gen.generate());
    gen.removeTaken(0);
    EXPECT_EQ(0, gen.generate());
}

TEST_F(TypeManagers, ParseJSON)
{
    int node_types = 6;
    int pin_types = 5;

    EXPECT_EQ(node_types, _NodeTypeManager.Types().size()) << "Expected " << node_types << " and got " << _NodeTypeManager.Types().size() << " node types.";
    EXPECT_EQ(pin_types, _PinTypeManager.Types().size()) << "Expected " << pin_types << " and got " << _PinTypeManager.Types().size() << " pin types.";
}

TEST_F(TypeManagers, Properties)
{
    EXPECT_EQ("power", _PinTypeManager.Types().at(0).value("name").toString());
    EXPECT_EQ(0, _PinTypeManager.TypeNames()["power"]);

    EXPECT_EQ("Socket", _NodeTypeManager.Types().at(0).value("name").toString());
    EXPECT_EQ(0, _NodeTypeManager.TypeNames()["Socket"]);
}