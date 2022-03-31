/**
 * @file Window.hpp
 * @author Yang Sun (yzs0057@auburn.edu)
 * @version 0.1
 * @date 2021-11-08
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once
#include"Circuit.h"
#include"Level.h"
#include "Function.hpp"
#include"Connection.h"
#include"SimulationStructures.hpp"
#include "SimulationNet.hpp"
#include"COPStructures.hpp"
#include"SimulationNet.hpp"
#include"SimulationStructures.hpp"
#include "SimulationNet.hpp"
#include <string>
#include <map>
#include<iostream>
#include<algorithm>
#include <cctype>
 /*
  * A window tool to extract a subcircuit from the benchmark circuit.
  *
  * This tool can also get sub-circuit data, for later training artificial neural networks
  *
  * @param _nodeType The type of nodes used in the circuit to be modified.
  * @param _lineType THe type of lines used in the circuit to be modified.
  * @param _valueType The type of value to be simulated.Example: Value / FaultyValue
  */
template <class _valueType, class _nodeType, class _lineType>
class Window {
public:

	/*
	 * Apply a given circuit and a given node as start point to go backtrace and record found nodes. 
	 *
	 * The limit number should only be 2^n-1 (e.g. 3, 7, 15)
	 *
	 * This will return found backtrace nodes.
	 *
	 * @param _c The circuit to backtrace.
	 * @param _n The node as the start point to backtrace.
	 * @param _limit The limit number of total backtrace node.
	 */
	static std::vector<_nodeType*> backtrace_node(Circuit* _c, _nodeType* _n, int _limit);

	/*
	 * Apply a given circuit and a given node as start point to go forward and record found nodes.
	 *
	 * The limit number should only be 2^n-1 (e.g. 3, 7, 15)
	 *
	 * This will return found forward nodes.
	 *
	 * @param _c The circuit to forward.
	 * @param _n The node as the start point to forward.
	 * @param _limit The limit number of total forward node.
	 */
	static std::vector<_nodeType*> forward_node(Circuit* _c, _nodeType* _n, int _limit);

	/*
	 * Apply a given circuit and given line as center point to generate a new sub-circuit.
	 *
	 * The limit number should only be 2^n-1 (e.g. 3, 7, 15)
	 *
	 * This will return generated sub-circuit.
	 *
	 * @param _c The circuit to generate sub-circuit.
	 * @param _line The line as the center point of sub-circuit.
	 * @param _limit The number of sub-ircuit size.
	 */
	Circuit* Getwindow(Circuit* _c, Levelized* _line, int _limit);

	/*
	 * Apply a given circuit and a given line as center point to 
	 * generate input feature for aritificial neural networks.
	 *
	 * The limit number should only be 2^n-1 (e.g. 3, 7, 15)
	 *
	 * This will return input features include CC, CO and gate type in vectorize format.
	 *
	 * @param _c The benchmark circuit not the sub-circuit.
	 * @param _line The line as the center point of sub-circuit.
	 * @param _limit The number of sub-ircuit size.
	 */
	 static std::vector< float> getdata(Circuit* _c, Levelized* _line, int _limit);
 
	 /*
	 * Apply a given circuit and given line to 
	 * create a map of lines' name and their controllability value.
	 * 
	 * The limit number should only be 2^n-1 (e.g. 3, 7, 15)
	 *
	 * This will return a map of lines'name and their controllability.
	 *
	 * @param _c The benchmark circuit not the sub-circuit.
	 * @param _line The line as the center point of sub-circuit.
	 * @param _limit The number of sub-ircuit size.
	 */
	 std::map<std::string, float>name_controllability(Circuit* _c, Levelized* _line, int _limit);

	 /*
	 * Apply a given circuit and a given line to 
	 * create a map of lines' name and their observability value.
	 * 
	 * The limit number should only be 2^n-1 (e.g. 3, 7, 15)
	 *
	 * This will return a map of lines'name and their observability.
	 *
	 * @param _c The benchmark circuit not the sub-circuit.
	 * @param _line The line as the center point of sub-circuit.
	 * @param _limit The number of sub-ircuit size.
	 */
	 std::map<std::string, float>name_observability(Circuit* _c, Levelized* _line, int _limit);
 
};


template <class _valueType, class _nodeType, class _lineType>
std::vector<_nodeType*>Window<_valueType, _nodeType, _lineType>::backtrace_node(Circuit* _c, _nodeType* _n, int _limit)
{
	if (_c == nullptr) {
		throw(" unvalid circuit\n");

	}
	if (_n == nullptr) {
		throw("unvalid node\n");

	}

	std::vector<_nodeType*>backtrace_nodes = {};
	backtrace_nodes.push_back(_n);

	int cur = 0;
	for (cur = 0; cur < backtrace_nodes.size(); cur++)
	{
		_nodeType* node = backtrace_nodes.at(cur);


		if (node == nullptr)
		{
			backtrace_nodes.push_back(nullptr);
			backtrace_nodes.push_back(nullptr);
		}
		else {

			std::string funcstring = node->name();
			if (funcstring == "pi")//pi
			{
				backtrace_nodes.push_back(nullptr);
				backtrace_nodes.push_back(nullptr);
			}
			else {

				std::unordered_set<Connecting*>node_ins = node->inputs();
				if (node_ins.size() == 1)  //only have one input
				{
					backtrace_nodes.push_back(nullptr);
				}
				for (Connecting* node_in : node_ins)
				{

					Connecting*l = *node_in->inputs().begin();
					_nodeType* s_item = dynamic_cast <_nodeType*>(l);
					if (s_item == nullptr)//in a fanout
					{
						std::unordered_set<Connecting*>l_node = l->inputs();
						for (Connecting* l_n : l_node)
						{
							_nodeType* l_nc = dynamic_cast <_nodeType*>(l_n);
							backtrace_nodes.push_back(l_nc);
						}

					}
					else {

						backtrace_nodes.push_back(s_item);

					}

				}

			}
		}

		while (backtrace_nodes.size() >= _limit)



		{
			return backtrace_nodes;
		}
	}

}


template <class _valueType, class _nodeType, class _lineType>
std::vector<_nodeType*> Window<_valueType, _nodeType, _lineType>::forward_node(Circuit* _c, _nodeType* _n, int _limit)

{
	if (_c == nullptr) {
		throw(" unvalid circuit\n");

	}
	if (_n == nullptr) {
		throw("unvalid node\n");

	}

	std::vector<_nodeType*>forward_nodes = {};//debug
	forward_nodes.push_back(_n);
	int cur = 0;
	for (cur = 0; cur < forward_nodes.size(); cur++)

	{
		_nodeType* node = forward_nodes.at(cur);

		if (node == nullptr)
		{
			forward_nodes.push_back(nullptr);
			forward_nodes.push_back(nullptr);

		}
		else {

			std::string funcstring = node->name();
			if (node->outputs().size() == 0)
			{

				forward_nodes.push_back(nullptr);
				forward_nodes.push_back(nullptr);

			}

			else {

				Connecting* node_outs = *node->outputs().begin();

				if (node_outs->outputs().size() == 1) {
					forward_nodes.push_back(nullptr);
				}

				for (Connecting* l : node_outs->outputs())

				{
					_nodeType* s_item = dynamic_cast <_nodeType*>(l);
					if (s_item == nullptr)//in a fanout
					{
						std::unordered_set<Connecting*>l_node = l->outputs();
						for (Connecting* l_n : l_node)
						{
							_nodeType* l_nc = dynamic_cast <_nodeType*>(l_n);
							forward_nodes.push_back(l_nc);
						}

					}
					else {

						forward_nodes.push_back(s_item);


					}


				}

			}


		}

		while (forward_nodes.size() >= _limit)
		{
			return forward_nodes;
		}

	}

}

template <class _valueType, class _nodeType, class _lineType>
Circuit* Window<_valueType, _nodeType, _lineType>::Getwindow(Circuit* _c, Levelized* _line, int _limit)

{

	std::vector<_nodeType*>backtrace_nodes = {};
	std::vector<_nodeType*>forward_nodes = {};

	Connecting*line_driver = *(_line->inputs()).begin();
	_nodeType* line_driver_n = dynamic_cast<_nodeType*>(line_driver);
	if (line_driver_n != nullptr)// line is a root
	{

		backtrace_nodes = backtrace_node(_c, line_driver_n, _limit);

		forward_nodes = forward_node(_c, line_driver_n, _limit);
	}

	else//line in fan-out
	{
		_lineType* line_driver_l = dynamic_cast<_lineType*>(line_driver);
		Connecting* line_driver_l_driver = *(line_driver_l->inputs()).begin();//root
		Connecting* line_driven = *(_line->outputs()).begin();
		_nodeType* begin_node_back = dynamic_cast<_nodeType*>(line_driver_l_driver);
		_nodeType* begin_node_forward = dynamic_cast<_nodeType*>(line_driven);

		backtrace_nodes = backtrace_node(_c, begin_node_back, _limit);

		forward_nodes = forward_node(_c, begin_node_forward, _limit);
	}


//connect line node
	std::unordered_set<_nodeType*>nodes_old = {};
	nodes_old.insert(backtrace_nodes.begin(), backtrace_nodes.end());
	nodes_old.insert(forward_nodes.begin(), forward_nodes.end());
	std::unordered_set<Connecting*>lines_old = {};

	std::unordered_set<_nodeType*>nodes_new = {};
	std::unordered_set<_lineType*>lines_new = {};
	std::map<Connecting*, _lineType*>oldtonewline;
	std::map<_nodeType*, _nodeType*>oldtonewnode;
	std::unordered_set<Connecting*>empty = {};

	//create new nodes
	for (_nodeType* node : nodes_old)
	{
		if (node != nullptr)
		{

			std::string funcstring = node->name();
			_nodeType* newnode;
			if (funcstring == "pi" || funcstring == "po")

			{
				newnode = new _nodeType(nullptr, funcstring);
			}

			else
			{
				newnode = new _nodeType(new BooleanFunction<_valueType>(funcstring), funcstring);

			}

			newnode->outputs(empty);
			newnode->inputs(empty);
			;
			nodes_new.emplace(newnode);
			oldtonewnode[node] = newnode;

			std::unordered_set<Connecting*>inputs = node->inputs();
			std::unordered_set<Connecting*>outputs = node->outputs();
			if (funcstring == "po")
			{
				lines_old.insert(inputs.begin(), inputs.end());

			}
			else if (funcstring == "pi")
			{
				lines_old.insert(outputs.begin(), outputs.end());

			}
			else {
				lines_old.insert(inputs.begin(), inputs.end());
				lines_old.insert(outputs.begin(), outputs.end());
			}

		}
	}
	_lineType*temm = nullptr;
	//create new line
	for (Connecting* line : lines_old)
	{

		std::string name = line->name();


		_lineType* copline = dynamic_cast<_lineType*>(line);
		_lineType*newlinecf = new _lineType(name);
		newlinecf->outputs(empty);
		newlinecf->inputs(empty);
		lines_new.emplace(newlinecf);
		oldtonewline[line] = newlinecf;
	}

	//connect node input
	for (_nodeType* node : nodes_old)
	{
		if (node != nullptr)
		{
			std::unordered_set<Connecting*>inputs = node->inputs();
			if (inputs.size() != 0)
			{
				std::unordered_set<_lineType*>new_inputs;
				//oldtonewnode.at(node)->inputs({});
				for (Connecting* in : inputs)
				{
					new_inputs.emplace(oldtonewline.at(in));

					oldtonewnode.at(node)->addInput(oldtonewline.at(in));
				}

			}


			//connect node output and fanout
			std::unordered_set<Connecting*>outputline = node->outputs();
			if (outputline.size() != 0)
			{
				for (Connecting* line : outputline)
				{

					oldtonewnode.at(node)->addOutput(oldtonewline.at(line));

					std::unordered_set<Connecting*>fanout = line->outputs();
					if (fanout.size() > 1)
					{
						for (Connecting* line_f : fanout)
						{

							auto it = oldtonewline.find(line_f);
							if (it != oldtonewline.end()) {

								oldtonewline.at(line)->addOutput(oldtonewline.at(line_f));
							}


						}

					}

				}
			}
		}
	}

	std::unordered_set<Levelized*>pis = {};
	std::unordered_set<Levelized*>pos = {};
	std::unordered_set<Levelized*>node_new1;
	std::unordered_set<Connecting*>sppis = {};   //not PI but in window it is pi
	std::unordered_set<Connecting*>sppos = {};   //not PO but in window it is po
	for (_nodeType* node : nodes_new)
	{
		if (node != nullptr)
		{

			node_new1.emplace(node);
			std::string funcstring_l = node->name();

			if (funcstring_l == "pi")
			{

				pis.emplace(node);



			}
			else if (funcstring_l == "po")
			{

				pos.emplace(node);



			}



			for (Connecting* line : node->inputs())
			{
				if (line->inputs().size() == 0)

				{
					sppis.emplace(line);
					_nodeType *pi = new _nodeType(nullptr, "pi");
					pi->outputs(empty);
					pi->inputs(empty);
					pis.emplace(pi);
					node_new1.emplace(pi);
					line->addInput(pi);


				}
			}
			for (Connecting* line : node->outputs())
			{
				if (line->outputs().size() == 0)
				{
					sppos.emplace(line);
					_nodeType *po = new _nodeType(nullptr, "po");

					po->outputs(empty);
					po->inputs(empty);
					pos.emplace(po);
					node_new1.emplace(po);
					line->addOutput(po);



				}
			}

		}
	}
	Circuit *window = new Circuit(node_new1, pis, pos);

	for (Connecting* l : lines_old)
	{
		_lineType* copline = dynamic_cast<_lineType*>(l);
		float oldccline = copline->controllability();
		float oldcoline = copline->observability();
		oldtonewline.at(l)->controllability(oldccline);
		oldtonewline.at(l)->observability(oldcoline);
	}
	for (_nodeType* n : nodes_old)
	{
		if (n != nullptr)
		{
			float oldcc = n->controllability();
			float oldco = n->observability();
			oldtonewnode.at(n)->controllability(oldcc);
			oldtonewnode.at(n)->observability(oldco);
		}
	}
	for (Connecting* l : sppis) {
		Connecting* pi = *l->inputs().begin();
		_nodeType* coppi = dynamic_cast<_nodeType*>(pi);
		_lineType* copline = dynamic_cast<_lineType*>(l);
		coppi->controllability(copline->controllability());
		coppi->observability(copline->observability());

	}

	return window;

}


template <class _valueType, class _nodeType, class _lineType>

std::vector< float> Window<_valueType, _nodeType, _lineType>::getdata(Circuit* _c, Levelized* _line, int _limit)


{

	std::vector<_nodeType*>backtrace_nodes = {};
	std::vector<_nodeType*>forward_nodes = {};
	std::unordered_set<Connecting*>line_ins = _line->inputs();
	Connecting*line_driver = *line_ins.begin();
	//Connecting*line_driver = *(line->inputs()).begin();
	_nodeType* line_driver_d = dynamic_cast<_nodeType*>(line_driver);
	if (line_driver_d != nullptr)
	{
		forward_nodes = forward_node(_c, line_driver_d, _limit);
		backtrace_nodes = backtrace_node(_c, line_driver_d, _limit);
	}
	//_lineType* line_driver2 = dynamic_cast<_lineType*>(line_driver);

	else
	{
		//_lineType* line_driver1 = dynamic_cast<_lineType*>(line_driver);
		Connecting* connecting_driver = *(line_driver->inputs()).begin();
		Connecting* connecting_driven = *(_line->outputs()).begin();
		_nodeType* node_driver = dynamic_cast<_nodeType*>(connecting_driver);
		_nodeType* node_driven = dynamic_cast<_nodeType*>(connecting_driven);
		forward_nodes = forward_node(_c, node_driven, _limit);

		backtrace_nodes = backtrace_node(_c, node_driver, _limit);

	}

	std::vector<Connecting*>lines = {};


	std::vector<_nodeType*>nodes_old = {};
	nodes_old.insert(nodes_old.end(), backtrace_nodes.begin(), backtrace_nodes.end());
	nodes_old.insert(nodes_old.end(), forward_nodes.begin(), forward_nodes.end());
	std::vector<float>netccco;
	COP *copline;

	//record inputs of nodes
	for (int i = 0; i < nodes_old.size(); i++)
	{
		_nodeType* node = nodes_old.at(i);
		if (node == nullptr)
		{
			lines.push_back(nullptr);
			lines.push_back(nullptr);
		}
		else
		{

			if (node != nullptr)
			{
				// Function<_valueType>* func1 = dynamic_cast<Function<_valueType>*>(node->function());
				std::string funcstring = node->name();
				if (funcstring == "constant" || funcstring == "pi")
				{
					lines.push_back(nullptr);
					lines.push_back(nullptr);
				}


				else if (node->inputs().size() == 1)
				{
					lines.push_back(nullptr);
					std::unordered_set<Connecting*>inputs = node->inputs();
					for (Connecting*line : inputs)
					{
						_lineType*Line = dynamic_cast<_lineType*>(line);
						if (Line == nullptr)
						{
							lines.push_back(nullptr);
						}
						else { lines.push_back(line); }
					}
				}
				else {

					std::unordered_set<Connecting*>inputs = node->inputs();
					for (Connecting*line : inputs)
					{
						_lineType*Line = dynamic_cast<_lineType*>(line);
						if (Line == nullptr)
						{
							lines.push_back(nullptr);
						}
						else { lines.push_back(line); }
					}
				}
			}
		}
	}


	//record ouputs of nodes

	for (_nodeType* node : nodes_old)
	{

		if (node == nullptr || (node->outputs()).size() == 0)
		{
			lines.push_back(nullptr);
		}
		else {

			std::unordered_set<Connecting*>outputs = node->outputs();
			for (Connecting* line : outputs)
			{
				_lineType*Line = dynamic_cast<_lineType*>(line);
				if (Line == nullptr)
				{
					lines.push_back(nullptr);
				}
				else

				{
					lines.push_back(line);
				}
			}
		}
	}




	//record cc and co of lines and gate type

	for (Connecting* line : lines)
	{
		if (line == nullptr)
		{


			netccco.push_back(0.5);
		}
		else {

			_lineType* copline = dynamic_cast<_lineType*>(line);

			float cc = copline->controllability();
			netccco.push_back(cc);
		}
	}


	for (Connecting* line : lines)
	{
		if (line == nullptr)
		{
			netccco.push_back(1);

		}
		else {
			_lineType* copline = dynamic_cast<_lineType*>(line);  //debug  nullptr
			float co = copline->observability();

			netccco.push_back(co);

		}
	}


	for (Connecting* node : nodes_old)
	{
		if (node == nullptr)
		{
			netccco.push_back(0);
			netccco.push_back(0);
			netccco.push_back(0);
			netccco.push_back(0);
		}
		else {
			_nodeType* simnode = dynamic_cast<_nodeType*>(node);

			std::string funcstring = simnode->name();
			if (funcstring == "and")
			{

				netccco.push_back(0);
				netccco.push_back(0);
				netccco.push_back(0);
				netccco.push_back(1);
			}
			else if (funcstring == "nand")
			{


				netccco.push_back(0);
				netccco.push_back(0);
				netccco.push_back(1);
				netccco.push_back(0);
			}
			else if (funcstring == "or")
			{

				netccco.push_back(0);
				netccco.push_back(0);
				netccco.push_back(1);
				netccco.push_back(1);
			}
			else if (funcstring == "nor")
			{

				netccco.push_back(0);
				netccco.push_back(1);
				netccco.push_back(0);
				netccco.push_back(0);
			}
			else if (funcstring == "xor")
			{

				netccco.push_back(0);
				netccco.push_back(1);
				netccco.push_back(0);
				netccco.push_back(1);
			}
			else if (funcstring == "xnor")
			{


				netccco.push_back(0);
				netccco.push_back(1);
				netccco.push_back(1);
				netccco.push_back(0);
			}
			else if (funcstring == "buf" || funcstring == "buff")
			{

				netccco.push_back(0);
				netccco.push_back(1);
				netccco.push_back(1);
				netccco.push_back(1);
			}
			else if (funcstring == "not")
			{

				netccco.push_back(1);
				netccco.push_back(0);
				netccco.push_back(0);
				netccco.push_back(0);
			}
			else if (funcstring == "pi" || funcstring == "tp_const_1" || funcstring == "tp_const_0")
			{

				netccco.push_back(1);
				netccco.push_back(0);
				netccco.push_back(0);
				netccco.push_back(1);
			}
			else if (funcstring == "po" || funcstring == "TP_observe")
			{

				netccco.push_back(1);
				netccco.push_back(0);
				netccco.push_back(1);
				netccco.push_back(0);
			}

		}
	}

	return netccco;


}

template <class _valueType, class _nodeType, class _lineType>

std::map<std::string, float>Window<_valueType, _nodeType, _lineType>::name_controllability(Circuit* _c, Levelized* _line, int _limit)
{
	std::vector<_nodeType*>backtrace_nodes = {};
	std::vector<_nodeType*>forward_nodes = {};

	Connecting*line_driver = *(_line->inputs()).begin();

	_nodeType* line_driver_d = dynamic_cast<_nodeType*>(line_driver);
	if (line_driver_d != nullptr)
	{
		forward_nodes = forward_node(_c, line_driver_d, _limit);
		backtrace_nodes = backtrace_node(_c, line_driver_d, _limit);

	}


	else
	{

		Connecting* connecting_driver = *(line_driver->inputs()).begin();
		Connecting* connecting_driven = *(_line->outputs()).begin();
		_nodeType* node_driver = dynamic_cast<_nodeType*>(connecting_driver);
		_nodeType* node_driven = dynamic_cast<_nodeType*>(connecting_driven);
		backtrace_nodes = backtrace_node(_c, node_driver, _limit);
		forward_nodes = forward_node(_c, node_driven, _limit);
	}

	std::vector<_nodeType*>nodes_old = {};
	nodes_old.insert(nodes_old.end(), backtrace_nodes.begin(), backtrace_nodes.end());
	nodes_old.insert(nodes_old.end(), forward_nodes.begin(), forward_nodes.end());
	std::map<std::string, float>name_controlability;
	COP *copline;
	for (_nodeType* node : nodes_old)
	{
		if (node != nullptr && (node->outputs()).size() != 0)
		{

			Connecting*output = *(node->outputs()).begin();
			_lineType* copline = dynamic_cast<_lineType*>(output);
			name_controlability[copline->name()] = copline->controllability();
		}
	}

	for (_nodeType* node : nodes_old)
	{
		if (node != nullptr && (node->inputs()).size() != 0)
		{
			for (Connecting*input : node->inputs())
			{
				std::string name = input->name();
				if (name_controlability.find(name) == name_controlability.end())
				{
					_lineType* copline = dynamic_cast<_lineType*>(input);
					name_controlability[copline->name()] = copline->controllability();
				}
			}
		}
	}

	return name_controlability;

}

template <class _valueType, class _nodeType, class _lineType>

std::map<std::string, float>Window<_valueType, _nodeType, _lineType>::name_observability(Circuit* _c, Levelized* _line, int _limit)
{
	std::vector<_nodeType*>backtrace_nodes = {};
	std::vector<_nodeType*>forward_nodes = {};

	Connecting*line_driver = *(_line->inputs()).begin();
	_nodeType* line_driver_d = dynamic_cast<_nodeType*>(line_driver);
	if (line_driver_d != nullptr)
	{
		forward_nodes = forward_node(_c, line_driver_d, _limit);

		backtrace_nodes = backtrace_node(_c, line_driver_d, _limit);

	}


	else
	{
		//_lineType* line_driver1 = dynamic_cast<_lineType*>(line_driver);
		Connecting* connecting_driver = *(line_driver->inputs()).begin();
		Connecting* connecting_driven = *(_line->outputs()).begin();
		_nodeType* node_driver = dynamic_cast<_nodeType*>(connecting_driver);
		_nodeType* node_driven = dynamic_cast<_nodeType*>(connecting_driven);
		forward_nodes = forward_node(_c, node_driven, _limit);
		backtrace_nodes = backtrace_node(_c, node_driver, _limit);

	}

	std::vector<_nodeType*>nodes_old = {};
	nodes_old.insert(nodes_old.end(), backtrace_nodes.begin(), backtrace_nodes.end());
	nodes_old.insert(nodes_old.end(), forward_nodes.begin(), forward_nodes.end());
	std::map<std::string, float>name_observability;
	COP *copline;
	for (_nodeType* node : nodes_old)
	{
		if (node != nullptr && (node->outputs()).size() != 0)
		{

			Connecting*output = *(node->outputs()).begin();
			_lineType* copline = dynamic_cast<_lineType*>(output);
			name_observability[copline->name()] = copline->observability();
		}
	}

	for (_nodeType* node : nodes_old)
	{
		if (node != nullptr && (node->inputs()).size() != 0)
		{
			for (Connecting*input : node->inputs())
			{
				std::string name = input->name();
				if (name_observability.find(name) == name_observability.end())
				{
					_lineType* copline = dynamic_cast<_lineType*>(input);
					name_observability[copline->name()] = copline->observability();
				}
			}
		}
	}
	return name_observability;

}





 