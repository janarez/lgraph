#include "VertexPermuter.h"

// finds l graph solution component by component
bool VertexPermuter::permuteVertices(void)
{
	size_t offset = 1;

	for (size_t c = 0; c < size_comp; ++c)
	{
		size_t size = comp_size[c];

		// adjacency matrix for the compoent
		setMatrix(offset, size);
		// find component solution
		if (!createGraph(offset, size))
			return false;

		offset += comp_size[c];
	}

	return true;
}

// creates Lgraph for component with vertex IDs offset to offset+n
bool VertexPermuter::createGraph(size_t offset, size_t n)
{	
	// class Permutation searches for the solution using adjacency matrix to cutdown possible permutations
	Permutation p(setNeighbors(offset, n),matrix);
	
	std::vector<size_t> result = p.lookForPermutation();

	// fail to find solution
	if (result.empty())
		return false;
	else
	{
		fillPermutation(offset, result);
		return true;
	}
}

// shift vertex IDs to start at 1
std::map<size_t, std::set<size_t>>& VertexPermuter::setNeighbors(size_t v, size_t n)
{
	new_neighbors.clear();
	std::set<size_t> temp;

	for (size_t row = v; row < v + n; ++row)
	{
		for (auto const& i : neighbors[row])
		{
			temp.insert(i - v + 1);
		}
		new_neighbors[row - v + 1] = temp;
		temp.clear();
	}

	return new_neighbors;
}

// sets adjacency matrix for component
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

// fills resulting permutation for one component
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

