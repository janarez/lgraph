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

	// component sizes
	std::vector<size_t> comp_size;

	// total number of vertices
	size_t size_per;
	// # of components
	size_t size_comp;

	// variable for matrix handling
	size_t a = 1;

	// loops through permutations
	bool createGraph(size_t offset, size_t n);

	// renames and truncates neighbors for a component
	std::map<size_t, std::set<size_t>>& prepareNeighbors(size_t start, size_t n, std::vector<size_t>& perm);

	// sets matrix for a given components - at the beggining
	void setMatrix(size_t start, size_t size);

	// fills permutation with correct permutation of one component
	void fillPermutation(size_t offset, std::vector<size_t>& perm);
	// finds next possible permutation to feed igraph
	std::vector<size_t>& nextPermutation(size_t n, std::vector<size_t>& p);
	// returns all BD edges that go against solvable matrix
	std::vector<size_t>& findAlWrongInMatrix(size_t A, size_t C, size_t n);

	// permutes matrix if no invalid situation is found, so that it corresponds to current permutation
	void permuteMatrix(std::vector<size_t>& p);
	
	// puts matrix into correct state up with regards to a
	std::vector<size_t>& handleInvalidInMatrix(std::vector<size_t>& incorrect, size_t C);

	// decides whether the new matrix should have ABDC or ACBD
	size_t switchBorD(size_t A, size_t B, size_t D, size_t C);


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