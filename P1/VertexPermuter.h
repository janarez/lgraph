#pragma once

#include "Permutation.h"

#include <vector>
#include <numeric>
#include <algorithm>
#include <utility>
#include <tuple>

// class that finds l graph by using Permutation class for each graph component
class VertexPermuter
{
	std::map<size_t, std::set<size_t>> neighbors;
	std::map<size_t, std::set<size_t>> new_neighbors;

	// adjacency matrix used to cut down # of possible permutations of each component
	std::vector<std::vector<size_t>> matrix;

	// component sizes
	std::vector<size_t> comp_size;

	// total number of vertices
	size_t size_per;
	// # of components
	size_t size_comp;

	// loops through permutations
	bool createGraph(size_t offset, size_t n);

	// sets matrix for a given components - at the beggining
	void setMatrix(size_t start, size_t size);

	// selects only neighbors from a given component
	std::map<size_t, std::set<size_t>>& setNeighbors(size_t offset, size_t n);

	// fills permutation with correct permutation of one component
	void fillPermutation(size_t offset, std::vector<size_t>& perm);

public:
	
	// constructed with map of neighbors stacked by components and component sizes
	VertexPermuter(std::map<size_t, std::set<size_t>> n, std::vector<size_t> c)
	{
		neighbors = n;
		comp_size = c;

		size_comp = comp_size.size();
		size_per = neighbors.size();

		permutation.resize(size_per + 1);
	}

	// resulting permutation; public so that igraph can be set when solution is found in gl
	std::vector<size_t> permutation;

	// represents functionality of the whole class
	bool permuteVertices();

};