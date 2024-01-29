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
#include <QtGraph/DNode>
#include <QtGraph/DGraph>
#include "private/models/nodespawndata.h"
#include "private/utilities/utility.h"
#include "private/helpers/idgenerator.h"
#include "private/models/pindata.h"

using namespace testing;
using namespace qtgraph;

class TestTypeManagers : public ::testing::Test
{
protected:
    TestTypeManagers() {}

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

class TestIdGenerator : public ::testing::Test
{
protected:
    TestIdGenerator() {}

    void SetUp() override { ASSERT_TRUE(ID::getTakenIDs().empty()); }
    void TearDown() override { ID::clear(); }
};



TEST(TestIPinData, ByteArrayConversions)
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

TEST(TestINodeSpawnData, ByteArrayConversions)
{
    QString str("Text");
    INodeSpawnData first(str);
    QByteArray arr1 = first.toByteArray();
    INodeSpawnData second = INodeSpawnData::fromByteArray(arr1);
    EXPECT_EQ(first, second);

    INodeSpawnData third(str, 1);
    QByteArray arr2 = third.toByteArray();
    INodeSpawnData fourth = INodeSpawnData::fromByteArray(arr2);
    EXPECT_EQ(third, fourth);
}

TEST(TestUtilities, Snapping)
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

TEST(TestUtilities, ParseToColor)
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

//TEST(TestUtilities, ParseStack)
//{
//    auto stack = Stack<IAction*>();
//
//    stack.push(new IAction(
//        EAction::Disconnection,
//        "Pin disconnection",
//        [](DGraph*, QVector<const void *>*){},
//        [](DGraph*, QVector<const void *>*){},
//        {}
//    ));
//
//    stack.push(new IAction(
//        EAction::Addition,
//        "Node addition",
//        [](DGraph*, QVector<const void *>*){},
//        [](DGraph*, QVector<const void *>*){},
//        {}
//    ));
//
//    EXPECT_EQ("[ PD, NA ]", parseStack(&stack));
//}

TEST_F(TestIdGenerator, General)
{
    EXPECT_EQ(0, ID::generate<DNode>());
    EXPECT_EQ(1, ID::generate<DNode>());
    EXPECT_EQ(2, ID::generate<DNode>());
    ID::removeTaken<DNode>(1);
    EXPECT_EQ(1, ID::generate<DNode>());
    EXPECT_EQ(3, ID::generate<DNode>());
}

TEST_F(TestIdGenerator, TypeSeparation)
{
    EXPECT_EQ(0, ID::generate<DNode>());
    EXPECT_EQ(1, ID::generate<DNode>());
    EXPECT_EQ(2, ID::generate<DNode>());

    EXPECT_EQ(0, ID::generate<DPin>());
    EXPECT_EQ(1, ID::generate<DPin>());

    EXPECT_EQ(3, ID::generate<DNode>());

    EXPECT_EQ(0, ID::generate<DGraph>());
}

TEST_F(TestTypeManagers, ParseJSON)
{
    int node_types = 6;
    int pin_types = 5;

    EXPECT_EQ(node_types, _NodeTypeManager.Types().size()) << "Expected " << node_types << " and got " << _NodeTypeManager.Types().size() << " node types.";
    EXPECT_EQ(pin_types, _PinTypeManager.Types().size()) << "Expected " << pin_types << " and got " << _PinTypeManager.Types().size() << " pin types.";
}

TEST_F(TestTypeManagers, Properties)
{
    EXPECT_EQ("power", _PinTypeManager.typeNameByID(0));
    EXPECT_EQ(0, _PinTypeManager.TypeNames()["power"]);

    EXPECT_EQ("Socket", _NodeTypeManager.typeNameByID(0));
    EXPECT_EQ(0, _NodeTypeManager.TypeNames()["Socket"]);
}