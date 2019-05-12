#pragma once

#include "LIntersect.h"

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>

// this class goes through vertex permutations to feed LIntersectionGraph
class Permutation
{
	LIntersectionGraph igraph;
	
	std::map<size_t, std::set<size_t>> neighbors;
	std::map<size_t, std::set<size_t>> new_neighbors;


	// adjacency matrix
	std::vector<std::vector<size_t>> matrix;

	// total number of vertices of permutation p
	size_t n;
	std::vector<size_t> p;

	// renames and truncates neighbors for a component
	std::map<size_t, std::set<size_t>>& prepareNeighbors();

	// finds next possible permutation to feed igraph
	bool nextPermutation(size_t v);

	bool newPermutation(std::vector<size_t>& usable);

	int checkMoveValidity(std::vector<size_t>& usable, size_t v);

public:

	// constructed with map of neighbors and adjacency matrix
	Permutation(std::map<size_t, std::set<size_t>>& nb, std::vector<std::vector<size_t>>& mx)
	{
		neighbors = nb;
		matrix = mx;
		n = matrix.size() - 1;

		igraph.initClass(n);

		p.resize(n);
	}

	// class functionality; if result not found returns empty permutation
	std::vector<size_t> lookForPermutation();
};