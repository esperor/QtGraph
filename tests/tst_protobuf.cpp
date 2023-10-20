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
protected:
    Protocolization() {}
    void SetUp() override 
    {
        graph = new LGraph();
        auto node1 = graph->addNode(QPoint(0, 10), "Node");
        auto node2 = graph->addNode(QPoint(100, 100), "Node");
        node1_id = node1->ID();
        node2_id = node2->ID();
        pin1_id = node1->addPin("pin1", EPinDirection::Out);
        pin2_id = node2->addPin("pin2", EPinDirection::In);
        IPinData pin1_data = node1->pin(pin1_id).value()->getData();
        IPinData pin2_data = node2->pin(pin2_id).value()->getData();
        ASSERT_TRUE(graph->connectPins(pin1_data, pin2_data)) << "Couldn't add connection";
        structure = graph->getStructure();
        file = "test_protocolization.graph";
    }

    void TearDown() override
    {
        delete graph;
    }

    LGraph *graph;
    std::string file;
    uint32_t node1_id, node2_id, pin1_id, pin2_id;
    protocol::Structure structure;
};

TEST_F(Protocolization, PinDirectionCompatibility)
{
    EPinDirection dir = EPinDirection::In;

    EXPECT_EQ((protocol::EPinDirection)dir, protocol::EPinDirection::IN);
}

TEST_F(Protocolization, Serialization)
{
    std::fstream out(file, std::ios::out | std::ios::trunc | std::ios::binary);
    ASSERT_TRUE(out.is_open());
    EXPECT_TRUE(graph->serialize(&out));
}

TEST_F(Protocolization, Deserialization)
{
    delete graph;
    graph = new LGraph();

    std::fstream in(file, std::ios::in | std::ios::binary);
    ASSERT_TRUE(in.is_open());
    EXPECT_TRUE(graph->deserialize(&in));
}

TEST_F(Protocolization, NodesData)
{
    EXPECT_EQ(graph->getNodeName(node1_id), "Node");
    EXPECT_EQ(graph->getNodeName(node2_id), "Node");
}

TEST_F(Protocolization, PinsData)
{
    EXPECT_EQ(graph->getPinText(node1_id, pin1_id), "pin1");
}

TEST_F(Protocolization, StructureData)
{
    auto deserializedStructure = graph->getStructure().mutable_edges();
    auto initialStructure = structure.mutable_edges();
    ASSERT_EQ(deserializedStructure->size(), initialStructure->size());

    for (int i = 0; i < deserializedStructure->size(); i++)
    {
        EXPECT_TRUE(MessageDifferencer::Equals(deserializedStructure->at(i), initialStructure->at(i)));
    }
}