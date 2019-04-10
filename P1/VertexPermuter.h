#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>


class VertexPermuter
{
	std::map<size_t, std::set<size_t>> neighbors;
	std::map<size_t, std::set<size_t>> new_neighbors;

	// adjacency matrix used to cut down # of possible permutations of each component
	std::vector<std::vector<size_t>> matrix;

	std::vector<size_t> comp_size;

	size_t size_per;
	size_t size_comp;

	bool createGraph(size_t offset, size_t n);
	std::map<size_t, std::set<size_t>>& prepareNeighbors(size_t start, size_t n, std::vector<size_t>& perm);

	void setMatrix(size_t start, size_t size);

	void fillPermutation(size_t offset, std::vector<size_t>& perm);

public:

	VertexPermuter(std::map<size_t, std::set<size_t>> n, std::vector<size_t> c)
	{
		neighbors = n;
		comp_size = c;

		size_comp = comp_size.size();
		size_per = neighbors.size();

		permutation.resize(size_per + 1);
	}

	// public so that igraph can be set when solution is found in gl
	std::vector<size_t> permutation;

	// represents functionality of the whole class
	bool permuteVertices();

};