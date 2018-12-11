#pragma once

#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <ostream>
#include <set>
#include <unordered_set>


class GraphLoader
{
	void removeZeroDegreeVertices();
	std::vector<size_t> zerodegree;

public:
	std::map<size_t, std::unordered_set<int>> neighbors;
	void load(std::istream& is);
	void loading(System::String^ file);
	
	std::unordered_set<int> returnAdjacent(System::String^ name);
	
	void registerVertex(size_t n);
	void registerEdge(System::String^ a, System::String^ b);
	void deleteVertex(System::String^ n);
	void deleteEdge(System::String^ a, System::String^ b);
};

class LShape
{
	size_t up;		// coordinates defining Lshape
	size_t bend;
	size_t side;

public:

	bool doIntersect(LShape& a, LShape& b) const;

	size_t getUp() const { return up; }
	size_t getBend()const { return bend; }
	size_t getSide()const { return side; }
	void setUp(size_t a) { up = a; return; }
	void setBend(size_t a) { bend = a; return; }
	void setSide(size_t a) { side = a; return; }

};

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
public:
	
	std::vector<LShape> shapes;
	LIntersectionGraph(std::map<size_t, std::unordered_set<int>>& neighbors);
	void printResult() const;
	bool createLGraph(void);
};