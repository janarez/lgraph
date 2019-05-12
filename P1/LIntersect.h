#pragma once

#include "PartialOrder.h"
#include "LShape.h"

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <set>
#include <numeric>

// L graph is represented in the form of matrix
// matrix dimension is #vertices X #vertices
// each L is is given by three coordinates

class LIntersectionGraph
{	
	// -1 is left, 1 is right, 0 is undecide
	std::vector<int> directions;
	std::vector<int> stops;
	std::vector<size_t> bends;

	size_t max = 0;
	LShape ls;
	PartialOrder cum;

	void deduceDirections(void);
	bool guessDirections(size_t counter);
	bool tryToFind(void);
	void deduceStopIntervals(void);
	bool guessStopIntervals(size_t counter, size_t neighbor);

	bool doPartialOrder(void);
	void fillShapes(void);

public:
	std::map<size_t, std::set<size_t>> neighbors;

	std::vector<LShape> shapes;

	// constructor for vertexPermuter
	LIntersectionGraph(size_t n);
	// empty constructor to hold information in graphLoader
	LIntersectionGraph() {};

	bool createLGraph(void);

	int returnDirection(size_t i) const;

	void initClass(size_t n);
};