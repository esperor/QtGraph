#include <gtest/gtest.h>

#include <QtGraph/Controller>

using namespace testing;
using namespace qtgraph;

/*

    Node selection and area selection aren't tested here
    because those actions are canvas-dependant by design
    therefore they are tested using QtTest together with 
    general canvas tests

*/

class TestActions : public ::testing::Test
{
public:
    TestActions() {}
    static void SetUpTestSuite()
    {
        controller = new Controller();
        node1 = controller->addNode(QPoint(0, 10), "Node 1");
        node2 = controller->addNode(QPoint(100, 100), "Node 2");
        node1_id = node1->ID();
        node2_id = node2->ID();
        pin1_id = node1->addPin("pin1", EPinDirection::In);
        pin2_id = node2->addPin("pin2", EPinDirection::Out);
    }

    static void TearDownTestSuite()
    {
        delete controller;
    }

    static Controller *controller;
    static DNode *node1, *node2;
    static uint32_t node1_id, node2_id, pin1_id, pin2_id;
};

Controller *TestActions::controller = nullptr;

DNode* TestActions::node1 = nullptr, * TestActions::node2 = nullptr;

uint32_t TestActions::node1_id = 0, TestActions::node2_id = 0, TestActions::pin1_id = 0, TestActions::pin2_id = 0;

TEST_F(TestActions, Count)
{
    EXPECT_EQ(controller->getStack()->size(), 2);
}

TEST_F(TestActions, PinConnection)
{
    IPinData pin1_data = node1->pin(pin1_id).value()->getData();
    IPinData pin2_data = node2->pin(pin2_id).value()->getData();
    controller->connectPins(pin1_data, pin2_data);

    auto &connections = controller->getGraph_const()->getConnections();
    EXPECT_TRUE(connections.contains(pin1_data, pin2_data));

    controller->disconnectPins(pin1_data, pin2_data);
    EXPECT_FALSE(connections.contains(pin1_data, pin2_data));

    controller->undo();
    EXPECT_TRUE(connections.contains(pin1_data, pin2_data));

    controller->undo();
    EXPECT_FALSE(connections.contains(pin1_data, pin2_data));
}

TEST_F(TestActions, SingleNodeDeletion)
{
    controller->removeNode(node1_id);
    EXPECT_FALSE(controller->nodes().contains(node1_id));

    controller->undo();
    EXPECT_TRUE(controller->nodes().contains(node1_id));
}

TEST_F(TestActions, MultipleNodesDeletion)
{
    auto node3 = controller->addNode(QPoint(-100, 100), "Node 3");
    controller->removeNodes({ node1_id, node2_id });
    EXPECT_FALSE(controller->nodes().contains(node1_id));
    EXPECT_FALSE(controller->nodes().contains(node2_id));
    EXPECT_FALSE(controller->nodes().empty());

    controller->undo();
    EXPECT_FALSE(controller->nodes().empty());
    EXPECT_TRUE(controller->nodes().contains(node1_id));
    EXPECT_TRUE(controller->nodes().contains(node2_id));

    controller->removeNodes({ node1_id, node2_id, node3->ID() });
    EXPECT_TRUE(controller->nodes().empty());

    controller->undo();
    EXPECT_FALSE(controller->nodes().empty());
}

TEST_F(TestActions, Clear)
{
    EXPECT_FALSE(controller->nodes().empty());
    controller->clear();
    EXPECT_TRUE(controller->nodes().empty());

    controller->undo();
    EXPECT_TRUE(controller->nodes().contains(node1_id));
    EXPECT_TRUE(controller->nodes().contains(node2_id));
}

TEST_F(TestActions, AddNode)
{
    auto node4 = controller->addNode(QPoint(-200, -200), "Node 4");
    uint32_t node4_id = node4->ID();
    EXPECT_TRUE(controller->nodes().contains(node4_id));

    controller->undo();
    EXPECT_FALSE(controller->nodes().contains(node4_id));
}
