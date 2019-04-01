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
#include <iterator>

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

	// put vertices in correct component
	int c_v1 = -1;
	int c_v2 = -1;
	size_t counter = 0;

	for (auto& i : components)
	{
		if (c_v1 == -1)
		{
			if (std::find(i.begin(), i.end(), v1) != i.end())
				c_v1 = counter;
		}
		if (c_v2 == -1)
		{
			if (std::find(i.begin(), i.end(), v2) != i.end())
				c_v2 = counter;
		}
		++counter;
	}

	// update components
	if (c_v1 == -1 && c_v2 == -1)
	{
		std::vector<size_t> newc{ v1, v2 };
		components.push_back(newc);
	}
	else if (c_v1 == -1)
		components[c_v2].push_back(v1);
	else if (c_v2 == -1)
		components[c_v1].push_back(v2);
	else if (c_v1 != c_v2)
	{
		components[c_v1].insert(components[c_v1].end(), components[c_v2].begin(), components[c_v2].end());
		components.erase(components.begin() + c_v2);
	}
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

bool GraphLoader::addNeighbors(size_t vertex)
{
	if (comp.empty())
		return false;
	
	for (auto u : neighbors[vertex])
	{
		auto pos = std::find(comp.begin(), comp.end(), u);
		if (pos != comp.end())
		{
			comp.erase(pos);
			if (!addNeighbors(u))
				return false;
		}		
	}
	return true;
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

	// find correct component
	size_t counter = 0;
	for (auto& i : components)
	{
		if (std::find(i.begin(), i.end(), v1) != i.end())
			break;
		++counter;
	}
	// new component? ... (v1, v2) was bridge
	comp = components[counter];
	bool decompose = addNeighbors(v1);
	if (decompose)
	{
		std::vector<size_t> new_comp;
		// set_differnce needs sorted vectors
		std::sort(components[counter].begin(), components[counter].end());
		std::sort(comp.begin(), comp.end());
		std::set_difference(components[counter].begin(), components[counter].end(), comp.begin(), comp.end(),
			std::inserter(new_comp, new_comp.begin()));
		components[counter] = comp;
		components.push_back(new_comp);
	}
	// else still one component

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


void GraphLoader::updatePermutation(void)
{

}

std::map<size_t, std::set<size_t>>& GraphLoader::permuteNeighbors()
{
	// updatePermutation(); // thro exception if not possible
	bool next = std::next_permutation(permutation.begin() + 1, permutation.end());
	//permutation = { 0, 2,3,7,9,1,6,5,10,8,4 };

	if (!next)
		throw std::exception("Tried all permutations...");

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


