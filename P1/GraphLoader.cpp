#include "GraphLoader.h"
#include "VertexPermuter.h"

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
	fixed_components = components;

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

	// rename vertices in components
	for (auto& i : fixed_components)
	{
		for (size_t v = 0; v < i.size(); ++v)
		{
			if (rename.count(i[v]))
				i[v] = rename[i[v]];
		}
	}
	setVertexID(rename);
	size_per = fixed_neighbors.size();
	permutation.resize(size_per+1);
	std::iota(permutation.begin(), permutation.end(), 0);
	stackByComponents();

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
		// use stoul to check for negative numbers in file
		int u1 = stoi(msclr::interop::marshal_as<std::string>(a));
		int u2 = stoi(msclr::interop::marshal_as<std::string>(b));

		if (u1 < 1 || u2 < 0)
			throw std::exception();
		else
		{
			v1 = u1;
			v2 = u2;
		}

	}
	catch (const std::exception&)
	{
		std::cerr << "Vertex value error" << std::endl;
		return;
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
	// delete all edges from vertex
	// done thriugh deleteEdge to handle new components
	while (neighbors[vertex].begin() != neighbors[vertex].end())
	{
		System::String^ s = gcnew System::String((*neighbors[vertex].begin()).ToString());
		deleteEdge(n, s);
	}

	// erase the empty set
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
	if (neighbors[v1].erase(v2))
		neighbors[v2].erase(v1);
	else
		return;

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
	auto pos = std::find(comp.begin(), comp.end(), v1);
	comp.erase(pos);
	bool decompose = addNeighbors(v1);


	if (decompose)
	{
		std::vector<size_t> new_comp;
		// set_differnce needs sorted vectors
		std::sort(components[counter].begin(), components[counter].end());
		std::sort(comp.begin(), comp.end());
		std::set_difference(components[counter].begin(), components[counter].end(), comp.begin(), comp.end(),
			std::inserter(new_comp, new_comp.begin()));
		if (comp.size() > 1)
			components[counter] = comp;
		else
			components.erase(components.begin() + counter);
		
		if (new_comp.size() > 1)
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

// organize components in a way that all vertices from given component have adjacent numbers ... [][][]
void GraphLoader::stackByComponents(void)
{
	// sort all components
	comp_sizes.clear();
	for (auto& i : fixed_components)
	{
		std::sort(i.begin(), i.end());
	}

	size_t count = fixed_components.size();
	for (size_t i = 0; i < count; ++i)
	{
		comp_sizes.push_back(fixed_components[i].size());
	}

	std::vector<size_t> rename(size_per+1);
	// vertex numbering starts at 1
	size_t offset = 1;

	for (size_t i = 0; i < count; ++i)
	{
		std::vector<size_t> component = fixed_components[i];
		for (size_t j = 0; j < comp_sizes[i]; ++j)
		{
			size_t v = component[j];
			rename[v] = j + offset;
		}

		offset += comp_sizes[i];
	}

	// rename neighbors
	new_neighbors.clear();
	std::set<size_t> temp;

	for (auto const& i : fixed_neighbors)
	{
		// rename the vertex in neighbors
		new_neighbors.insert(std::make_pair(rename[i.first], fixed_neighbors[i.first]));
		// rename all edges
		for (size_t edge : new_neighbors[rename[i.first]])
		{
			temp.insert(rename[edge]);
		}
		new_neighbors[rename[i.first]] = temp;
		temp.clear();
	}

	resetVertexID(rename);
	fixed_neighbors = new_neighbors;
	return;
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

// when graph is loaded from file all drawings are cleared
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
			System::String^ left = gcnew System::String(line.c_str());
			
			getline(ss, line, ' ');
			System::String^ right = gcnew System::String(line.c_str());	

			// not necessary to register yet unused vertex thanks to the map []operator
			registerEdge(left, right);
		}
		catch (const std::exception&)
		{
			std::cerr << "File must be formatted such that there is one edge on each line. Vertices are numbers." << std::endl;
			return;
		}	
	}
}

size_t GraphLoader::returtVertexID(size_t n)
{
	return vertexID[n];
}

void GraphLoader::resetVertexID(std::vector<size_t>& permutation)
{
	std::vector<size_t> temp(size_per+1);
	for (size_t i = 1; i <= size_per; ++i)
	{
		temp[permutation[i]] = vertexID[i];
	}
	vertexID = temp;
	return;
}

bool GraphLoader::permuteNeighbors()
{
	VertexPermuter vp(fixed_neighbors, comp_sizes);
	if (vp.permuteVertices())
	{
		igraph.initClass(size_per);
		igraph.neighbors = setPermutation(vp.permutation);
		return igraph.createLGraph();
	}
	else
		return false;
}

std::map<size_t, std::set<size_t>>& GraphLoader::setPermutation(std::vector<size_t>& permutation)
{
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

	resetVertexID(permutation);
	return new_neighbors;
}


