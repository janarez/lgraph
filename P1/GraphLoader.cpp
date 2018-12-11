#include "GraphLoader.h"

#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <ostream>
#include <algorithm>
#include <set>
#include <numeric>
#include <iostream>
#include <exception>
#include<iostream>
#include<istream>
#include<fstream>

// for converting from System::String^ to std::string
#include <msclr\marshal_cppstd.h>

void GraphLoader::removeZeroDegreeVertices()
{
	size_t shouldbe = 1;
	std::map<size_t, size_t> rename;

	for (auto i : neighbors)
	{
		if (i.first != shouldbe)
		{
			rename.insert(std::make_pair(i.first, shouldbe));
		}
		++shouldbe;
	}

	for (auto const& i : rename)
	{
		neighbors.insert(std::make_pair(rename[i.first], neighbors[i.first]));
		neighbors.erase(i.first);
		for (auto& j : neighbors[rename[i.first]])
		{
			neighbors[j].erase(i.first);
			neighbors[j].insert(rename[i.first]);

		}
	}
}

void GraphLoader::registerVertex(size_t n)
{
	zerodegree.push_back(n);
	return;
}

void GraphLoader::registerEdge(System::String^ a, System::String^ b)
{
	size_t v1, v2;

	try
	{
		v1 = stoi(msclr::interop::marshal_as<std::string>(a));
		v2 = stoi(msclr::interop::marshal_as<std::string>(b));
	}
	catch (const std::exception&)
	{
		throw std::exception("Unexpected Error");
	}
	neighbors[v1].insert(v2);
	neighbors[v2].insert(v1);
	return;
}

void GraphLoader::deleteVertex(System::String^ n)
{
	size_t vertex;
	try
	{
		vertex = stoi(msclr::interop::marshal_as<std::string>(n));
	}
	catch (const std::exception&)
	{
		throw std::exception("Unexpected Error");
	}
	neighbors.erase(vertex);
	// delete from zerodegree as well
}

void GraphLoader::deleteEdge(System::String^ a, System::String^ b)
{
	size_t v1, v2;

	try
	{
		v1 = stoi(msclr::interop::marshal_as<std::string>(a));
		v2 = stoi(msclr::interop::marshal_as<std::string>(b));
	}
	catch (const std::exception&)
	{
		throw std::exception("Unexpected Error");
	}
	neighbors[v1].erase(v2);
	neighbors[v2].erase(v1);
	return;
}

std::unordered_set<int> GraphLoader::returnAdjacent(System::String^ name)
{
	size_t vertex;
	try
	{
		vertex = stoi(msclr::interop::marshal_as<std::string>(name));
	}
	catch (const std::exception&)
	{
		throw std::exception("Unexpected Error");
	}
	return neighbors[vertex];
}

void GraphLoader::loading(System::String^ f)
{
	std::string name = msclr::interop::marshal_as<std::string>(f);
	std::ifstream file(name);
	if (!file)
	{
		std::cerr << "Could not open file" << std::endl;
		return;
	}

	load(file);
}

void GraphLoader::load(std::istream& is)
{
	std::stringstream ss;
	std::string line;

	while (getline(is, line))
	{
		ss.clear();
		ss.str(line);
		// knowing there are only two numbers on each line
		getline(ss, line, ' ');
		int left = stoi(line);
		getline(ss, line, ' ');
		int right = stoi(line);
		neighbors[left].insert(right);
		neighbors[right].insert(left);
	}
	removeZeroDegreeVertices();
}

bool LShape::doIntersect(LShape& a, LShape& b) const
{
	if ((b.bend <= a.bend) && ((a.up <= b.side && a.up >= b.up) || (a.up >= b.side && a.up <= b.up)))
		return true;
	else if ((a.bend <= b.bend) && ((b.up <= a.side && b.up >= a.up) || (b.up >= a.side && b.up <= a.up)))
		return true;
	else
		return false;
}
