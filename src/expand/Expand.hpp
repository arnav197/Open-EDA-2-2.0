/**
 * @file Expand.hpp
 * @author Yang Sun (yzs0057@auburn.edu)
 * @version 0.1
 * @date 2021-11-08
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once
#define Expand_h
#include "Circuit.h"
#include "Function.hpp"
#include "SimulationNet.hpp"
#include "SimulationStructures.hpp"
#include "SimulationNet.hpp"
#include "Level.h"
#include "Net.hpp"
#include "Connection.h"
#include <string>
#include <utility> 
#include <set>
#include <map>
 
 /*
  * An Expand tool to convert circuit into a certain format.
  *
  * After expanding, the circuit gate inputs are no more than two and fan-outs are no more than two,
  * and return this coverted circuit.
  *
  * This converted circuit is for generating arificial neural networks input features.
  *
  * @param _nodeType The type of nodes used in the circuit to be modified.
  * @param _lineType THe type of lines used in the circuit to be modified.
  * @param _valueType The type of value to be simulated.Example: Value / FaultyValue
 */
template <class _valueType, class _nodeType, class _lineType>
class Expand {
public:
	/*
	 * Apply a given circuit to expand in.
	 *
	 * After expand_in, this circuit gate input number is no more than two
	 *
	 * This will return modified circuit.
	 *
	 * @param _c The circuit to perform expand in.
	 */
	Circuit* Expand_in(Circuit* _c);

	/*
	 * Apply a given circuit to expand out.
	 *
	 * After expand_out, this circuit gate fan-out number is no more than two
	 *
	 * This will return modified circuit.
	 *
	 * @param _c The circuit to perform expand out.
	 */
	Circuit* Expand_out(Circuit*_c);

	/*
	 * Apply a given circuit and a given node to check if this node needs to be expanded in.
	 *
	 * If the node inputs are more than two, return true, else return false.
	 *
	 * @param _c The circuit to perform expand in check.
	 * @param _node The node to perform expand in check. _node should in _c.
	 */
	static bool Expand_in_Check(Circuit*_c, Levelized* _node);

	/*
	 * Apply a given circuit and a given node to check if this node needs to be expanded out.
	 *
	 * If the node fan-outs are more than two, return true, else return false.
	 *
	 * @param _c The circuit to perform expand out check.
	 * @param _node The node to perform expand out check. _node should in _c.
	 */
	static bool Expand_out_Check(Circuit*_c, Levelized* _node);

	/*
	 * Apply a given circuit and a given node to expand out.
	 *
	 * This will return modified circuit.
	 *
	 * @param _c The circuit to perform expand out.
	 * @param _node The node to perform expand out. _node should in _c.
	 */
	static Circuit* Expand_nodeout(Circuit*_c, Levelized* _node);

	/*
	 * Apply a given circuit and a given node to expand in.
	 *
	 * This will return modified circuit.
	 *
	 * @param _c The circuit to perform expand in.
	 * @param _node The node to perform expand in. _node should in _c.
	 */
	static  Circuit* Expand_nodein(Circuit*_c, Levelized* _node);


};

template <class _valueType, class _nodeType, class _lineType>
Circuit* Expand<_valueType, _nodeType, _lineType>::Expand_in(Circuit* _c)
{
	std::unordered_set<Levelized*> nodes = _c->nodes();

	for (Levelized* node : nodes)
	{
		if (Expand_in_Check(_c, node) == false) {

			_c = Expand_nodein(_c, node);
		}
	}

	return _c;
}

template <class _valueType, class _nodeType, class _lineType>
Circuit* Expand<_valueType, _nodeType, _lineType>::Expand_out(Circuit*_c)

{

	std::unordered_set<Levelized*> nodes = _c->nodes();

	for (Levelized* node : nodes)
	{

		if (Expand_out_Check(_c, node) == false)
		{
			_c = Expand_nodeout(_c, node);
		}
	}

	return _c;

}

template <class _valueType, class _nodeType, class _lineType>
bool Expand<_valueType, _nodeType, _lineType>::Expand_in_Check(Circuit*_c, Levelized* _node)
{

	if (_c == nullptr) {
		throw("the circuit is not valid");
	}
	if (_node == nullptr) {
		throw("the node is not valid ");
	}

	size_t num = _node->inputs().size();
	std::string funcstring = _node->name();	//change into string form

	if (!funcstring.compare("buf") || !funcstring.compare("not")) {
		if (num == 1)
			return true;


		else {
			throw("wrong simnode");
		}
	}


	else if (!funcstring.compare("and") || !funcstring.compare("nand") || !funcstring.compare("or") || !funcstring.compare("nor") || !funcstring.compare("xor") || !funcstring.compare("xnor"))
	{
		if (num >= 3)

			return false;

		else
			return true;
	}
}

template <class _valueType, class _nodeType, class _lineType>

bool Expand<_valueType, _nodeType, _lineType>::Expand_out_Check(Circuit*_c, Levelized* _node)
{

	if (_c == nullptr) {
		throw("the circuit is not valid");
	}
	if (_node == nullptr) {
		throw("the node is not valid ");
	}


	std::unordered_set<Connecting*>lines_out = _node->outputs();
	for (Connecting* line : lines_out)

	{
		std::unordered_set<Connecting*>line_fanout = line->outputs();
		int num = line_fanout.size();

		if (num >= 3)
		{

			return false;
		}
		else
		{
			return true;
		}

	}

}



template <class _valueType, class _nodeType, class _lineType>
Circuit* Expand<_valueType, _nodeType, _lineType>::Expand_nodeout(Circuit*_c, Levelized* _node)
{

	std::vector<Connecting*>nodes = {};
	nodes.push_back(_node);
	std::unordered_set<Connecting*>empty = {};
	for (int num = 0; num < nodes.size(); num++)
	{

		_nodeType* sn = dynamic_cast<_nodeType*>(nodes.at(num));

		if (Expand_out_Check(_c, sn) == false)
		{
			
			Connecting*lines_out = *sn->outputs().begin();
			std::string name = lines_out->name();
			std::unordered_set<Connecting*>line_fanout_ = lines_out->outputs();
			std::vector<_lineType*>line_fanout = {};
			for (Connecting* line : line_fanout_)
			{
				line->removeInput(lines_out);
				line_fanout.push_back(dynamic_cast<_lineType*>(line));
			}

			lines_out->outputs(empty);

			const int num_total = line_fanout.size();
			const int num_half = num_total / 2;
			_lineType* simline = dynamic_cast<_lineType*>(lines_out);

			Net<_lineType, _nodeType>net(simline);

			std::string funcstring = "buf";

			_nodeType* newnode1 = new _nodeType(
				new BooleanFunction<_valueType>(funcstring),
				funcstring
			);
			newnode1->inputs(empty);
			newnode1->outputs(empty);
			_c->addNode(newnode1);
			newnode1->outputs(empty);
			lines_out->addOutput(newnode1);
			nodes.push_back(newnode1);


			_lineType*newline1 = new _lineType(name + "*");
			newline1->inputs(empty);
			newline1->outputs(empty);
			newline1->outputs(empty);
			newnode1->addOutput(newline1);
			//Net<COPFaultyLine<bool>, COPFaultyNode<bool>>net1(newline1);
			int i = 0;
			newline1->addOutput(line_fanout.at(i));
			i++;
			Net < _lineType, _nodeType>net1(newline1);

			while (i < line_fanout.size())
			{

				while (i < num_half)
				{

					net1.addFanout(line_fanout.at(i));


					i++;
				}
				break;
			}


			_nodeType* newnode2 = new _nodeType(
				new BooleanFunction<_valueType>(funcstring),
				funcstring
			);
			newnode2->inputs(empty);
			newnode2->outputs(empty);
			newnode2->outputs(empty);
			_c->addNode(newnode2);
			nodes.push_back(newnode2);
			net.addFanout(newnode2);
			int a = net.fanoutNodes().size();

			_lineType*newline2 = new _lineType(name + "**");
			newline2->inputs(empty);
			newline2->outputs(empty);
			newnode2->addOutput(newline2);
			newline2->addOutput(line_fanout.at(i));
			i++;
			Net<_lineType, _nodeType>net2(newline2);
			while (i < line_fanout.size())

			{
				while (i <= num_total - 1)
				{

					net2.addFanout(line_fanout.at(i));

					i++;
				}
				
			}


		}

	}


	return _c;

}


template <class _valueType, class _nodeType, class _lineType>
Circuit* Expand<_valueType, _nodeType, _lineType>::Expand_nodein(Circuit*_c, Levelized* _node)

{
	std::unordered_set<Connecting*>empty = {};


	if ((Expand_in_Check(_c, _node)) == false)
	{
		_nodeType* _sn = dynamic_cast<_nodeType*>(_node);

		Function<_valueType>* func = dynamic_cast<Function<_valueType>*>(_sn->function());
		std::string funcstring = func->string();

		std::string newfuncstring;
		if ((funcstring)[0] == 'n')
		{
			newfuncstring = funcstring.erase(0, 1);
			funcstring = func->string();
		}

		else {
			newfuncstring = funcstring;
		}
		std::unordered_set<Connecting*>lines = _node->inputs();

		std::vector<Connecting*>lines_;

		std::unordered_set<Levelized*>newnodes;

		int j = 0;

		for (Connecting* line : lines)
		{
			line->removeOutput(_node);
			lines_.push_back(line);

		}

		for (j = 0; j < lines_.size();)
		{

			_nodeType* newnode = new _nodeType(
				new BooleanFunction<_valueType>(newfuncstring),
				newfuncstring
			);
			newnode->outputs(empty);
			newnode->inputs(empty);
			std::string str1 = std::to_string(j);
			_c->addNode(newnode);
			newnodes.emplace(newnode);

			newnode->addInput(lines_.at(j));
			j++;
			newnode->addInput(lines_.at(j));


			std::string name = lines_.at(j)->name();
			_lineType*newline = new _lineType(name + "~" + str1);
			newline->inputs(empty);
			newline->outputs(empty);

			newnode->addOutput(newline);

			lines_.push_back(newline);
			j++;

			if (j == (lines_.size() - 2))

			{

				lines_.at(j)->addOutput(_node);


				lines_.at(j + 1)->addOutput(_node);

				return _c;


			}

		}

	}

	return _c;

}
