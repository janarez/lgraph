#pragma once

#include "LIntersect.h"
#include "VertexPermuter.h"

#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <ostream>
#include <algorithm>
#include <set>
#include <numeric>
#include <exception>
#include <iostream>
#include <istream>
#include <fstream>
#include <iterator>

// for converting from System::String^ to std::string
#include <msclr\marshal_cppstd.h>

// class for loading graph into a map of vertex : neighbors
class GraphLoader
{
	// stores zero degree vertices
	std::vector<size_t> zerodegree;
	
	// helper variables for loaded neighbors and loaded components
	std::map<size_t, std::set<size_t>> fixed_neighbors;
	std::vector<std::vector<size_t>> fixed_components;

	// converts between currently assigned vetex number and the loaded one
	std::vector<size_t> vertexID;
	std::vector<size_t> fixed_vertexID;

	// stores permutation of vertices in l graph
	std::vector<size_t> permutation;
	size_t size_per;

	size_t vertexCount;
	
	// changes vertexID vector for each permutation
	void resetVertexID(std::vector<size_t>& rename);
	void setVertexID(std::map<size_t, size_t>& rename);

	// one component, helper variable for recursive addNeighbors
	std::vector<size_t> comp;
	std::vector<size_t> comp_sizes;

	// recursive function that traverses neighbors of vertex and returns true if comp remains nonempty
	bool addNeighbors(size_t vertex);
	// creates initial permutation in which components are separately next to each other
	void stackByComponents(void);

	std::map<size_t, std::set<size_t>>& setPermutation(std::vector<size_t>& permutation);

public:
	std::map<size_t, std::set<size_t>> neighbors;
	std::map<size_t, std::set<size_t>> new_neighbors;
	
	// list of vertices in each component
	std::vector<std::vector<size_t>> components;

	// from file graph loading
	void load(std::istream& is);
	void loading(System::String^ file);

	std::set<size_t> returnAdjacent(System::String^ name);

	// handling graph from UI
	void registerVertex(size_t n);
	void registerEdge(System::String^ a, System::String^ b);
	void deleteVertex(System::String^ n);
	void deleteEdge(System::String^ a, System::String^ b);

	// get rid of zero degree vertices
	std::map<size_t, std::set<size_t>>& removeZeroDegreeVertices();

	void setVertexCount(size_t n);
	size_t returtVertexID(size_t n);

	bool permuteNeighbors();

	LIntersectionGraph igraph;
};

