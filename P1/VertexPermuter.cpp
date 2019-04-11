# include "VertexPermuter.h"
# include "LIntersect.h"

#include <vector>
#include <numeric>
#include <algorithm>

bool VertexPermuter::permuteVertices(void)
{
	size_t offset = 1;

	for (size_t c = 0; c < size_comp; ++c)
	{
		size_t size = comp_size[c];

		setMatrix(offset, size);
		if (!createGraph(offset, size))
			return false;

		offset += comp_size[c];
	}

	return true;
}

bool VertexPermuter::createGraph(size_t offset, size_t n)
{
	LIntersectionGraph igraph(n);
	std::vector<size_t> p(n);

	// try default permutation before proceeding, because it's often a solution
	std::iota(p.begin(), p.end(), 1);

	do
	{
		igraph.neighbors = prepareNeighbors(offset, n, p);

		if (igraph.createLGraph())
		{
			fillPermutation(offset, p);
			return true;
		}
		
		// here all the matrix magic will be done;
		p = nextPermutation(n, p);

	} while (!p.empty());
	
	return false;
}

std::vector<size_t>& VertexPermuter::nextPermutation(size_t n, std::vector<size_t>& p)
{
	std::vector<size_t> incorrect;

	for (size_t A = a; A < n; ++A)
	{
		for (size_t C = A; C < n; ++C)
		{
			if (matrix[A][C])
			{
				incorrect = findAlWrongInMatrix(A, C, n);

				// invalid situations
				if (!incorrect.empty())
				{
					// a will be set by this function as well
					return handleInvalidInMatrix(incorrect, C);
				}
			}
		}
	}

	// no invalid situations
	if (incorrect.empty())
	{
		if (std::next_permutation(p.begin(), p.end()))
		{
			// this function also sets a
			// this should be able to change the first vertex as well
			permuteMatrix(p);
			return p;
		}
		else
		{
			// returning empty vector = solution not found
			return incorrect;
		}
	}
}

std::vector<size_t>& VertexPermuter::findAlWrongInMatrix(size_t A, size_t C, size_t n)
{
	std::vector<size_t> incorrect;

	for (size_t B = A+1; B < C; ++B)
	{
		for (size_t D = C+1; D < n; ++D)
		{
			if (matrix[B][D] && !matrix[B][C] && !matrix[C][D] && !matrix[A][B])
			{
				size_t result = switchBorD(A, B, D, C);
				if (result)
					incorrect.push_back(result);
				else
				{
					throw std::exception("Cannot be solved");
				}
			}
		}
	}
	return incorrect;
}

size_t VertexPermuter::switchBorD(size_t A, size_t B, size_t D, size_t C)
{
	auto b = matrix[B].begin();
	auto d = matrix[D].begin();

	size_t bAB = std::count(b + A + 1, b + B, 1);
	size_t bBC = std::count(b + B + 1, b + C, 1);
	size_t bCD = std::count(b, matrix[B].end(), 1) - bAB - bBC - 1;


	size_t dAB = std::count(d + A + 1, b + B, 1);
	size_t dBC = std::count(d + B + 1, b + C, 1);
	size_t dCD = std::count(d, matrix[D].end(), 1) - dAB - dBC;

	if (inB > 0)
	{
		if (outB > 0)
			// here C must be moved before first neighbor of B
			return 0;
		else if (outD == 0)
			return D;
		else
			return 0;
	}
	else if (outB > 0)
	{
		if (inD == 0)
			return B;
		else
			return 0;
	}
	else
	{
		if (outD > 0)
			return B;
		else
			return D;
	}
}

void VertexPermuter::permuteMatrix(std::vector<size_t>& p)
{
	return;
}

std::vector<size_t>& VertexPermuter::handleInvalidInMatrix(std::vector<size_t>& incorrect, size_t C)
{
	
	return permutation;
}



void VertexPermuter::setMatrix(size_t start, size_t size)
{
	// ignore zeroth row / column
	matrix.resize(size + 1);

	for (size_t i = 1; i <= size; ++i)
		matrix[i].resize(size + 1);

	for (size_t i = start; i < start + size; ++i)
	{
		size_t curr_v = start;
		for (auto v : neighbors[i])
		{
			if (v != curr_v)
			{
				while (v != curr_v)
				{
					matrix[i-start+1][curr_v-start + 1] = 0;
					++curr_v;
				}
			}
			matrix[i-start+1][v-start + 1] = 1;
			curr_v = v + 1;
		}
	}
	return;
}

std::map<size_t, std::set<size_t>>& VertexPermuter::prepareNeighbors(size_t start, size_t n, std::vector<size_t>& p)
{
	new_neighbors.clear();
	std::set<size_t> temp;

	for (size_t row = start; row < start + n; ++row)
	{
		for (auto const& i : neighbors[row])
		{
			temp.insert(p[i - start]);
		}
		new_neighbors[row - start + 1] = temp;
		temp.clear();
	}

	return new_neighbors;
}

void VertexPermuter::fillPermutation(size_t offset, std::vector<size_t>& perm)
{
	size_t counter = offset;
	for (size_t i : perm)
	{
		permutation[counter] = i + offset - 1;
		++counter;
	}
	return;
}

