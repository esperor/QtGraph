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
        PinData pin1_data = node1->pin(pin1_id)->toStrongRef()->getData();
        PinData pin2_data = node2->pin(pin2_id)->toStrongRef()->getData();
        ASSERT_TRUE(canvas->addConnection(pin1_data, pin2_data)) << "Couldn't add connection";
        structure = canvas->getStructure();
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

TEST_F(Protocolization, PinsData)
{
    EXPECT_EQ(canvas->getPinText(node1_id, pin1_id), "pin1");
}

TEST_F(Protocolization, StructureData)
{
    EXPECT_EQ(canvas->getStructure(), structure);
}