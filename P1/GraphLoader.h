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
	std::vector<size_t> zerodegree;
	std::map<size_t, std::set<size_t>> fixed_neighbors;
	std::vector<size_t> vertexID;
	std::vector<size_t> fixed_vertexID;


	std::vector<size_t> permutation;
	size_t size_per;

	size_t vertexCount;
	
	void resetVertexID(void);
	void setVertexID(std::map<size_t, size_t>& rename);

	// list of vertices in each component
	std::vector<size_t> comp;
	bool addNeighbors(size_t vertex);

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

	void updatePermutation(void);

	std::map<size_t, std::set<size_t>>& permuteNeighbors();
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