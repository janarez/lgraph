#pragma once

#include "LIntersect.h"

#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <ostream>
#include <set>
#include <unordered_set>


class GraphLoader
{
	std::vector<size_t> zerodegree;
	std::map<size_t, std::set<size_t>> fixed_neighbors;
	std::vector<std::vector<size_t>> fixed_components;

	std::vector<size_t> vertexID;
	std::vector<size_t> fixed_vertexID;


	std::vector<size_t> permutation;
	size_t size_per;

	size_t vertexCount;
	
	void resetVertexID(std::vector<size_t>& rename);
	void setVertexID(std::map<size_t, size_t>& rename);

	// list of vertices in each component
	std::vector<size_t> comp;
	bool addNeighbors(size_t vertex);
	std::vector<size_t> comp_sizes;
	void stackByComponents(void);

	std::map<size_t, std::set<size_t>>& setPermutation(std::vector<size_t>& permutation);

public:
	std::map<size_t, std::set<size_t>> neighbors;
	std::map<size_t, std::set<size_t>> new_neighbors;

	std::vector<std::vector<size_t>> components;

	void load(std::istream& is);
	void loading(System::String^ file);

	std::set<size_t> returnAdjacent(System::String^ name);

	void registerVertex(size_t n);
	void registerEdge(System::String^ a, System::String^ b);
	void deleteVertex(System::String^ n);
	void deleteEdge(System::String^ a, System::String^ b);

	std::map<size_t, std::set<size_t>>& removeZeroDegreeVertices();

	void setVertexCount(size_t n);
	size_t returtVertexID(size_t n);

	bool permuteNeighbors();

	LIntersectionGraph igraph;
};

