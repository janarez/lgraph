#include "Permutation.h"

std::vector<size_t> Permutation::lookForPermutation() 
{
	// try default permutation before proceeding, because it's often a solution
	std::iota(p.begin(), p.end(), 1);

	igraph.neighbors = prepareNeighbors();
	bool exists = igraph.createLGraph();
	bool next = true;

	if (exists)
		return p;

	// look for viable permutation here
	for (size_t i = 1; i <= n; ++i)
	{
		p[0] = i;
		if (nextPermutation(i))
		{
			std::vector<size_t> result(n);
			auto start = p.begin();
			auto end = p.end();

			// resulting permutation
			for (size_t i = 1; i <= n; ++i)
			{
				result[i - 1] = std::find(start, end, i) - start + 1;
			}
			return result;
		}
	}

	// all possibilities were tried without success
	p.clear();
	return p;
}

bool Permutation::nextPermutation(size_t v)
{
	std::vector<size_t> usable(n - 1);

	// available vertices 1..,v-1,v+1,..n backwards
	if (n == v)
		std::iota(usable.begin(), usable.end(), 1);
	else
	{
		std::iota(usable.begin(), usable.begin() + v - 1, 1);
		std::iota(usable.begin() + v - 1, usable.end(), v + 1);
	}

	std::reverse(usable.begin(), usable.end());

	return newPermutation(usable);
}

bool Permutation::newPermutation(std::vector<size_t>& usable)
{
	size_t m = usable.size();

	if (m == 1)
	{
		p[n - 1] = usable[0];
		// test permutation here
		igraph.neighbors = prepareNeighbors();
		return igraph.createLGraph();
	}
	else
	{
		// maximum number of steps needed to find next element
		int pos = checkMoveValidity(usable, usable[m - 1]);

		if (pos == -1)
		{
			p[n - m] = usable[m - 1];
			usable.pop_back();
			if (newPermutation(usable))
				return true;
			else
			{
				usable.push_back(p[n - m]);
				for (int j = m - 2; j >= 0; --j)
				{
					size_t temp = usable[j];
					size_t v = usable[m - 1];

					if (temp < v) 
					{
						return false;
					}

					usable[j] = v;
					usable[m-1] = temp;
					if (newPermutation(usable))
						return true;
				}
				return false;
			}	
		}
		// remove collision
		else
		{
			size_t temp = usable[m - 1];
			if (temp > usable[m-2])
				return false;
			usable.pop_back();
			usable.insert(usable.begin() + pos, temp);
			if (newPermutation(usable))
				return true;
		}
	}
	return false;
}

int Permutation::checkMoveValidity(std::vector<size_t>& usable, size_t C)
{
	size_t m = usable.size() - 1;
	size_t n = this->n - m - 1;

	for (size_t A = 0; A < n - 1; ++A)
	{
		if (!matrix[p[A]][C])
			continue;

		for (size_t B = A + 1; B < n; ++B)
		{
			if (!matrix[p[B]][C] && !matrix[p[A]][p[B]])
			{
				for (size_t D = 0; D <= m; ++D)
				{
					if (matrix[p[B]][usable[D]] && !matrix[C][usable[D]])
					{
						return D;
					}
				}
			}
		}

	}

	return -1;
}

std::map<size_t, std::set<size_t>>& Permutation::prepareNeighbors()
{
	new_neighbors.clear();

	auto start = p.begin();
	auto end = p.end();

	std::set<size_t> temp;

	for (size_t row = 1; row <= n; ++row)
	{
		for (auto const& i : neighbors[row])
		{
			temp.insert(std::find(start, end, i) - start + 1);
		}
		new_neighbors[std::find(start, end, row) - start + 1] = temp;
		temp.clear();
	}

	return new_neighbors;
}