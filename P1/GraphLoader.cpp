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

std::map<size_t, std::set<size_t>>& GraphLoader::removeZeroDegreeVertices()
{
	fixed_neighbors = neighbors;
	zerodegree.clear();
	size_t shouldbe = 1;
	std::map<size_t, size_t> rename;

	for (auto i : fixed_neighbors)
	{
		if (i.second.empty())
		{
			zerodegree.push_back(i.first);
			continue;
		}
		if (i.first != shouldbe)
		{
			rename.insert(std::make_pair(i.first, shouldbe));
		}
		++shouldbe;
	}
	for (auto const i : zerodegree)
		fixed_neighbors.erase(i);

	for (auto const& i : rename)
	{
		// rename the vertex in neighbors
		fixed_neighbors.insert(std::make_pair(rename[i.first], fixed_neighbors[i.first]));
		fixed_neighbors.erase(i.first);
		// rename the vertex in all edges
		for (auto& j : fixed_neighbors[rename[i.first]])
		{
			fixed_neighbors[j].erase(i.first);
			fixed_neighbors[j].insert(rename[i.first]);
		}
	}
	setVertexID(rename);
	size_per = fixed_neighbors.size();
	permutation.resize(size_per+1);
	std::iota(permutation.begin(), permutation.end(), 0);
	return fixed_neighbors;
}

void GraphLoader::setVertexCount(size_t n)
{
	vertexCount = n;
	return;
}

void GraphLoader::setVertexID(std::map<size_t, size_t>& rename)
{
	vertexID.resize(vertexCount + 1);
	std::iota(vertexID.begin(), vertexID.end(), 0);
	
	for (auto const& i : rename)
	{
		vertexID[i.second] = i.first;
	}

	fixed_vertexID = vertexID;
}


void GraphLoader::registerVertex(size_t n)
{
	neighbors[n];
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
	// delete vertex from edges
	for (size_t u : neighbors[vertex])
		neighbors[u].erase(vertex);
	neighbors.erase(vertex);
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

std::set<size_t> GraphLoader::returnAdjacent(System::String^ name)
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
		try
		{
			getline(ss, line, ' ');
			int left = stoi(line);
			getline(ss, line, ' ');
			int right = stoi(line);
			neighbors[left].insert(right);
			neighbors[right].insert(left);
		}
		catch (const std::exception&)
		{
			throw std::exception("File must be formatted such that there is one edge on each line. Vertices are numbers.");
		}	
	}
}

size_t GraphLoader::returtVertexID(size_t n)
{
	return vertexID[n];
}

void GraphLoader::resetVertexID(void)
{
	std::vector<size_t> temp(size_per+1);
	for (size_t i = 1; i <= size_per; ++i)
	{
		temp[permutation[i]] = fixed_vertexID[i];
	}
	vertexID = temp;
	return;
}

std::map<size_t, std::set<size_t>>& GraphLoader::permuteNeighbors()
{
	// updatePermutation(); // thro exception if not possible
	std::next_permutation(permutation.begin() + 1, permutation.end());
	
	new_neighbors.clear();
	std::set<size_t> temp;

	for (auto const& i : fixed_neighbors)
	{
		// rename the vertex in neighbors
		new_neighbors.insert(std::make_pair(permutation[i.first], fixed_neighbors[i.first]));
		// rename all edges
		for (size_t edge : new_neighbors[permutation[i.first]])
		{
			temp.insert(permutation[edge]);
		}
		new_neighbors[permutation[i.first]] = temp;
		temp.clear();
	}

	resetVertexID();

	// throw std::exception("...");
	return new_neighbors;
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


