#pragma once

#include "GraphLoader.h"

#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <ostream>
#include <set>
#include <unordered_set>

// L graph is represented in the form of matrix
// matrix dimension is #vertices X #vertices
// each L is is given by three coordinates

class LIntersectionGraph
{
	// trying to create Lgraph
	std::vector<size_t> combination;
	std::vector<size_t> permutation;
	std::set<size_t> totest;

	size_t max = 0;
	LShape ls;
	GraphLoader gl;

	bool forbiddenEdgeCrossing(void);
	bool testEachPair(void);
	bool updateCombination();
	void insertPointsIntoL(void);
	bool updatePermutation();

	void deduceDirections(void);
	void guessDirections(size_t counter);
	bool tryToFind(void);
	void deduceStopIntervals(void);
	void doPartialOrder(void);

public:
	
	std::vector<LShape> shapes;
	// -1 is left, 1 is right, 0 is undecide
	std::vector<int> directions;
	std::vector<int> stops;

	LIntersectionGraph(std::map<size_t, std::set<size_t>>& neighbors);
	void printResult() const;
	bool createLGraph(void);
};