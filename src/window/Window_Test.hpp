#pragma once
#include "gtest/gtest.h"
#include "window/Window.hpp"
#include "simulation/Simulator.hpp"
#include "simulation/SimulationStructures.hpp"
#include "parser/Parser.hpp"
#include "Circuit.h"
#include "cop/COPStructures.hpp"

class WindowTest : public::testing::Test {
public:

	
	void SetUp() override {
		
	}
	Parser<COPLine<Value<bool>>, COPNode<Value<bool>>, Value<bool>> parser;
	Circuit* circuit = parser.Parse("c17.bench");
	Window<Value<bool>, COPNode < Value<bool>>, COPLine<Value<bool>>>*window;


};


// static std::vector<_nodeType*> backtrace_node(Circuit* _c, _nodeType* _n, int _limit)
TEST_F(WindowTest, TEST01)
{
	Levelized*node_temp = *circuit->nodes().begin();
	COPNode < Value<bool>>* sn_temp = dynamic_cast <COPNode<Value<bool>>*>(node_temp);
	std::vector<COPNode < Value<bool>>*>backtrace_nodes = window->backtrace_node(circuit, sn_temp, 5);
	EXPECT_EQ(backtrace_nodes.size(), 5);
}

// static std::vector<_nodeType*> forward_node(Circuit* _c, _nodeType* _n, int _limit)
TEST_F(WindowTest, TEST02)
{
	Levelized*node_temp = *circuit->nodes().begin();
	COPNode < Value<bool>>* sn_temp = dynamic_cast <COPNode<Value<bool>>*>(node_temp);
	std::vector<COPNode < Value<bool>>*>forward_nodes = window->backtrace_node(circuit, sn_temp, 5);
	EXPECT_EQ(forward_nodes.size(), 5);
}


// Circuit* Getwindow(Circuit* _c, _nodeType* _n, int _limit)
TEST_F(WindowTest, TEST03)
{
	std::unordered_set<Levelized*> nodes = circuit->nodes();

	for (Levelized* n : nodes)
	{
		if (n->outputs().size() > 0)
		{
			Connecting* output = *(n->outputs()).begin();
			if (output->name() == "7")
			{
				Levelized * output_l = dynamic_cast <Levelized*>(output);
				Circuit* subcircuit = window->Getwindow(circuit, output_l, 7);
				EXPECT_EQ(subcircuit->nodes().size(), 6);
			}
		}
	}

	
}




//static std::vector< float> getdata(Circuit* _c, Levelized* line, int _limit)
TEST_F(WindowTest, TEST04)
{
	Levelized*node_temp = *circuit->nodes().begin();
	Connecting*line_temp = *node_temp->outputs().begin();
	COPLine < Value<bool>>* sl_temp = dynamic_cast <COPLine<Value<bool>>*>(line_temp);
	std::vector< float>data = window->getdata(circuit, sl_temp, 5);
	EXPECT_EQ(data.size(), 100);
}



