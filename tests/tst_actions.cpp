#include <gtest/gtest.h>

#include <QtGraph/Controller>

using namespace testing;
using namespace qtgraph;

class TestActions : public ::testing::Test
{
public:
    TestActions() {}
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
    static uint32_t node1_id, node2_id, pin1_id, pin2_id;
};

Controller *TestActions::controller = new Controller();

uint32_t TestActions::node1_id = 0, TestActions::node2_id = 0, TestActions::pin1_id = 0, TestActions::pin2_id = 0;

TEST_F(TestActions, SimpleSelection)
{
    controller->selectNode(node1_id);
    EXPECT_EQ(QSet<uint32_t>{ node1_id }, controller->getSelectedNodes());

    controller->undo();
    EXPECT_TRUE(controller->getSelectedNodes().empty());
}
