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

public:
	std::map<size_t, std::set<size_t>> neighbors;
	void load(std::istream& is);
	void loading(System::String^ file);

	std::set<size_t> returnAdjacent(System::String^ name);

	void registerVertex(size_t n);
	void registerEdge(System::String^ a, System::String^ b);
	void deleteVertex(System::String^ n);
	void deleteEdge(System::String^ a, System::String^ b);

	void removeZeroDegreeVertices();
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