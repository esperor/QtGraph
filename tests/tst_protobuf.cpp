#include <gtest/gtest.h>

#include "QtGraph/GraphLib.h"
#include "QtGraph/DataClasses/pindata.h"
#include "QtGraph/GraphWidgets/Abstracts/abstractpin.h"
#include "QtGraph/GraphWidgets/canvas.h"

#include <fstream>

#include "pin.pb.h"
#include "structure.pb.h"

using namespace testing;
using namespace GraphLib;

class Protocolization : public ::testing::Test
{
protected:
    Protocolization() {}
    void SetUp() override 
    {
        canvas = new Canvas();
        auto node1 = canvas->addBaseNode(QPoint(0, 10), "Node").toStrongRef();
        auto node2 = canvas->addBaseNode(QPoint(100, 100), "Node").toStrongRef();
        node1_id = node1->ID();
        node2_id = node2->ID();
        pin1_id = node1->addPin("pin1", PinDirection::Out);
        pin2_id = node2->addPin("pin2", PinDirection::In);
        canvas->addConnection(node1[pin1_id]->getData(), node2[pin2_id]->getData());
        structure = canvas->getStucture();
        file = "test_protocolization.graph";
    }

    void TearDown() override
    {
        delete canvas;
    }

    Canvas *canvas;
    std::string file;
    uint32_t node1_id, node2_id, pin1_id, pin2_id;
    protocol::Structure structure;
};

TEST_F(Protocolization, PinDirectionCompatibility)
{
    PinDirection dir = PinDirection::In;
    protocol::PinDirection p_dir = (protocol::PinDirection)dir;

    EXPECT_EQ(p_dir, protocol::PinDirection::IN);
}

TEST_F(Protocolization, Serialization)
{
    std::fstream out(file, std::ios::out | std::ios::trunc | std::ios::binary);
    ASSERT_TRUE(out.is_open());
    EXPECT_TRUE(canvas->serialize(&out));
}

TEST_F(Protocolization, Deserialization)
{
    delete canvas;
    canvas = new Canvas();

    std::fstream in(file, std::ios::in | std::ios::binary);
    ASSERT_TRUE(in.is_open());
    EXPECT_TRUE(canvas->deserialize(&in));
}

TEST_F(Protocolization, NodesData)
{
    EXPECT_EQ(canvas->getNodeName(node1_id), "Node");
    EXPECT_EQ(canvas->getNodeName(node2_id), "Node");
}

TEST_F(Protocolization PinsData)
{
    EXPECT_EQ(canvas[node1_id][pin1_id]->getText() == "pin1");
}

TEST_F(Protocolization, StructureData)
{
    EXPECT_EQ(canvas->getStructure(), structure);
}