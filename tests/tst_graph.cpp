#include <gtest/gtest.h>

#include <QtGraph/LGraph>
#include <QSignalSpy>

using namespace testing;
using namespace qtgraph;

class TestGraph : public ::testing::Test
{
protected:
    void SetUp() override
    {
        graph = new LGraph();

        auto *node1 = graph->addNode(QPoint(0,0), "Node 1");
        auto *node2 = graph->addNode(QPoint(-100, -100), "Node 2");
        uint32_t pin1 = node1->addPin("Pin 1", EPinDirection::In);
        uint32_t pin2 = node2->addPin("Pin 2", EPinDirection::Out);

        graph->connectPins(node1->pin(pin1).value()->getData(), node2->pin(pin2).value()->getData()); 
    }

    void TearDown() override
    {
        delete graph;
    }

    LGraph *graph;
};


TEST_F(TestGraph, Clear)
{
    graph->clear();
    EXPECT_FALSE(graph->containsNode(0));
    EXPECT_TRUE(graph->getConnections().empty());
}

TEST_F(TestGraph, NodeRemoved)
{
    QSignalSpy spy(graph, &LGraph::onNodeRemoved);
    graph->removeNode(0);
    EXPECT_EQ(spy.count(), 1);
}