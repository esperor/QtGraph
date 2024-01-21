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
        initial = new Controller();
        auto node1 = initial->addNode(QPoint(0, 10), "Node");
        auto node2 = initial->addNode(QPoint(100, 100), "Node");
        node1_id = node1->ID();
        node2_id = node2->ID();
        pin1_id = node1->addPin("pin1", EPinDirection::In);
        pin2_id = node2->addPin("pin2", EPinDirection::Out);
        IPinData pin1_data = node1->pin(pin1_id).value()->getData();
        IPinData pin2_data = node2->pin(pin2_id).value()->getData();
        initial->connectPins(pin1_data, pin2_data);
        file = "test_protocolization.initial";
    }

    static void TearDownTestSuite()
    {
        delete initial;
        delete deserialized;
    }

    static Controller *initial;
    static Controller *deserialized;
    static std::string file;
    static uint32_t node1_id, node2_id, pin1_id, pin2_id;
};

Controller *TestProtocolization::initial = new Controller();
Controller *TestProtocolization::deserialized = new Controller();

std::string TestProtocolization::file = std::string();
uint32_t TestProtocolization::node1_id = 0, TestProtocolization::node2_id = 0, TestProtocolization::pin1_id = 0, TestProtocolization::pin2_id = 0;


TEST_F(TestProtocolization, PinDirectionCompatibility)
{
    EPinDirection dir = EPinDirection::In;

    EXPECT_EQ((protocol::EPinDirection)dir, protocol::EPinDirection::IN);
}

TEST_F(TestProtocolization, Serialization)
{
    std::fstream out(file, std::ios::out | std::ios::trunc | std::ios::binary);
    ASSERT_TRUE(out.is_open());
    EXPECT_TRUE(initial->serialize(&out));
}

TEST_F(TestProtocolization, Deserialization)
{
    std::fstream in(file, std::ios::in | std::ios::binary);
    ASSERT_TRUE(in.is_open());
    EXPECT_TRUE(deserialized->deserialize(&in));
}

TEST_F(TestProtocolization, NodesData)
{
    EXPECT_EQ(deserialized->getGraph_const()->getNodeName(node1_id), "Node");
    EXPECT_EQ(deserialized->getGraph_const()->getNodeName(node2_id), "Node");
}

TEST_F(TestProtocolization, PinsData)
{
    EXPECT_EQ(deserialized->getGraph_const()->getPinText(node1_id, pin1_id), "pin1");
}

TEST_F(TestProtocolization, StructureData)
{
    const auto &deserializedStructure = deserialized->getGraph_const()->getConnections();
    const auto &initialStructure = initial->getGraph_const()->getConnections();

    ASSERT_EQ(deserializedStructure.size(), initialStructure.size());
    
    std::ranges::for_each(deserializedStructure.asKeyValueRange(), [&](std::pair<const IPinData&, const IPinData&> pair){
        EXPECT_TRUE(initialStructure.contains(pair.first, pair.second));
    });
}