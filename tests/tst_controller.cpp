#include <gtest/gtest.h>

#include "private/logics/controller.h"
#include <QPoint>
#include <QSignalSpy>

using namespace testing;
using namespace qtgraph;

class TestController : public ::testing::Test
{
protected:
    void SetUp() override
    {
        controller = new Controller();

        auto *node1 = controller->addNode(QPoint(0,0), "Node 1");
        auto *node2 = controller->addNode(QPoint(-100, -100), "Node 2");
        uint32_t pin1 = node1->addPin("Pin 1", EPinDirection::In);
        uint32_t pin2 = node2->addPin("Pin 2", EPinDirection::Out);

        controller->connectPins(node1->pin(pin1).value()->getData(), node2->pin(pin2).value()->getData());
    }

    void TearDown() override
    {
        delete controller;
    }

    Controller *controller;
};


TEST_F(TestController, Clear)
{
    controller->clear();
    EXPECT_FALSE(controller->getGraph_const()->containsNode(0));
    EXPECT_TRUE(controller->getGraph_const()->getConnections().empty());
}

//TEST_F(TestController, NodeRemoved)
//{
//    QSignalSpy spy(controller->getGraph_const(), &DGraph::nodeRemoved);
//    controller->removeNode(0);
//    EXPECT_EQ(spy.count(), 1);
//}