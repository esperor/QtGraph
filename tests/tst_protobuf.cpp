#include <gtest/gtest.h>

#include <QtGraph/IPinData>
#include <QtGraph/DPin>
#include <QtGraph/Controller>

#include <fstream>

#include "pin.pb.h"
#include "structure.pb.h"

using namespace testing;
using namespace qtgraph;

class TestProtocolization : public ::testing::Test
{
public:
    TestProtocolization() {}
    static void SetUpTestSuite()
    {
        controller = new Controller();
        auto node1 = controller->addNode(QPoint(0, 10), "Node");
        auto node2 = controller->addNode(QPoint(100, 100), "Node");
        node1_id = node1->ID();
        node2_id = node2->ID();
        pin1_id = node1->addPin("pin1", EPinDirection::In);
        pin2_id = node2->addPin("pin2", EPinDirection::Out);
        IPinData pin1_data = node1->pin(pin1_id).value()->getData();
        IPinData pin2_data = node2->pin(pin2_id).value()->getData();
        controller->connectPins(pin1_data, pin2_data);
        file = "test_protocolization.graph";
        initialStructure = controller->getGraph_const()->getConnections();
    }

    static void TearDownTestSuite()
    {
        delete controller;
    }

    static Controller *controller;
    static std::string file;
    static uint32_t node1_id, node2_id, pin1_id, pin2_id;
    static QMultiMap<IPinData, IPinData> initialStructure;
};

Controller *TestProtocolization::controller = nullptr;

std::string TestProtocolization::file = std::string();
uint32_t TestProtocolization::node1_id = 0, TestProtocolization::node2_id = 0, TestProtocolization::pin1_id = 0, TestProtocolization::pin2_id = 0;

QMultiMap<IPinData, IPinData> TestProtocolization::initialStructure = {};


TEST_F(TestProtocolization, PinDirectionCompatibility)
{
    EPinDirection dir = EPinDirection::In;

    EXPECT_EQ((protocol::EPinDirection)dir, protocol::EPinDirection::IN);
}

TEST_F(TestProtocolization, Serialization)
{
    std::fstream out(file, std::ios::out | std::ios::trunc | std::ios::binary);
    ASSERT_TRUE(out.is_open());
    EXPECT_TRUE(controller->serialize(&out));
    delete controller;
    controller = new Controller();
}

TEST_F(TestProtocolization, Deserialization)
{
    std::fstream in(file, std::ios::in | std::ios::binary);
    ASSERT_TRUE(in.is_open());
    EXPECT_TRUE(controller->deserialize(&in));
}

TEST_F(TestProtocolization, NodesData)
{
    EXPECT_EQ(controller->getGraph_const()->getNodeName(node1_id), "Node");
    EXPECT_EQ(controller->getGraph_const()->getNodeName(node2_id), "Node");
}

TEST_F(TestProtocolization, PinsData)
{
    EXPECT_EQ(controller->getGraph_const()->getPinText(node1_id, pin1_id), "pin1");
}

TEST_F(TestProtocolization, StructureData)
{
    const auto &deserializedStructure = controller->getGraph_const()->getConnections();

    ASSERT_EQ(deserializedStructure.size(), initialStructure.size());
    
    std::ranges::for_each(deserializedStructure.asKeyValueRange(), [&](std::pair<const IPinData&, const IPinData&> pair){
        EXPECT_TRUE(initialStructure.contains(pair.first, pair.second));
    });
}