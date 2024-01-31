#include <gtest/gtest.h>

#include <QtGraph/IPinData>
#include <QtGraph/DPin>
#include <QtGraph/Controller>

#include <fstream>

#include "pin.pb.h"
#include "structure.pb.h"

using namespace testing;
using namespace qtgraph;

class TestIntegration : public ::testing::Test
{
public:
    TestIntegration() {}
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
    }

    static void TearDownTestSuite()
    {
        delete controller;
    }

    static Controller *controller;
    static std::string file;
    static uint32_t node1_id, node2_id, pin1_id, pin2_id;
};

Controller *TestIntegration::controller = nullptr;

uint32_t TestIntegration::node1_id = 0, TestIntegration::node2_id = 0, TestIntegration::pin1_id = 0, TestIntegration::pin2_id = 0;

TEST_F(TestIntegration, ProtobufActions)
{
    std::string file = "test_integration.graph";
    std::fstream out(file, std::ios::out | std::ios::trunc | std::ios::binary);
    ASSERT_TRUE(out.is_open());
    ASSERT_TRUE(controller->serialize(&out));
    out.close();
    delete controller;
    controller = new Controller();

    std::fstream in(file, std::ios::in | std::ios::binary);
    ASSERT_TRUE(in.is_open());
    ASSERT_TRUE(controller->deserialize(&in));
    in.close();

    EXPECT_EQ(1, controller->getGraph_const()->getConnections().size());
    controller->removeNode(node1_id);
    EXPECT_EQ(1, controller->nodes().size());
    EXPECT_FALSE(controller->nodes().contains(node1_id));
    controller->undo();
    EXPECT_TRUE(controller->nodes().contains(node1_id));
}