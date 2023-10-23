#include <gtest/gtest.h>

#include "private/models/pindata.h"
#include <QtGraph/LPin>
#include <QtGraph/LGraph>

#include <fstream>

#include "pin.pb.h"
#include "structure.pb.h"

#include <google/protobuf/util/message_differencer.h>

using namespace testing;
using namespace qtgraph;

using google::protobuf::util::MessageDifferencer;

class Protocolization : public ::testing::Test
{
public:
    Protocolization() {}
    static void SetUpTestSuite()
    {
        initial = new LGraph();
        auto node1 = initial->addNode(QPoint(0, 10), "Node");
        auto node2 = initial->addNode(QPoint(100, 100), "Node");
        node1_id = node1->ID();
        node2_id = node2->ID();
        pin1_id = node1->addPin("pin1", EPinDirection::Out);
        pin2_id = node2->addPin("pin2", EPinDirection::In);
        IPinData pin1_data = node1->pin(pin1_id).value()->getData();
        IPinData pin2_data = node2->pin(pin2_id).value()->getData();
        ASSERT_TRUE(initial->connectPins(pin1_data, pin2_data)) << "Couldn't add connection";
        file = "test_protocolization.initial";
    }

    static void TearDownTestSuite()
    {
        delete initial;
        delete deserialized;
    }

    static LGraph *initial;
    static LGraph *deserialized;
    static std::string file;
    static uint32_t node1_id, node2_id, pin1_id, pin2_id;
};

LGraph *Protocolization::initial = new LGraph();
LGraph *Protocolization::deserialized = new LGraph();

std::string Protocolization::file = std::string();
uint32_t Protocolization::node1_id = 0, Protocolization::node2_id = 0, Protocolization::pin1_id = 0, Protocolization::pin2_id = 0;


TEST_F(Protocolization, PinDirectionCompatibility)
{
    EPinDirection dir = EPinDirection::In;

    EXPECT_EQ((protocol::EPinDirection)dir, protocol::EPinDirection::IN);
}

TEST_F(Protocolization, Serialization)
{
    std::fstream out(file, std::ios::out | std::ios::trunc | std::ios::binary);
    ASSERT_TRUE(out.is_open());
    EXPECT_TRUE(initial->serialize(&out));
}

TEST_F(Protocolization, Deserialization)
{
    std::fstream in(file, std::ios::in | std::ios::binary);
    ASSERT_TRUE(in.is_open());
    EXPECT_TRUE(deserialized->deserialize(&in));
}

TEST_F(Protocolization, NodesData)
{
    EXPECT_EQ(deserialized->getNodeName(node1_id), "Node");
    EXPECT_EQ(deserialized->getNodeName(node2_id), "Node");
}

TEST_F(Protocolization, PinsData)
{
    EXPECT_EQ(deserialized->getPinText(node1_id, pin1_id), "pin1");
}

TEST_F(Protocolization, StructureData)
{
    const auto &deserializedStructure = deserialized->getConnections();
    const auto &initialStructure = initial->getConnections();

    ASSERT_EQ(deserializedStructure.size(), initialStructure.size());
    
    std::ranges::for_each(deserializedStructure.asKeyValueRange(), [&](std::pair<const IPinData&, const IPinData&> pair){
        EXPECT_TRUE(initialStructure.contains(pair.first, pair.second));
    });
}