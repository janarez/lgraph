#include "Permutation.h"

std::vector<size_t> Permutation::lookForPermutation() 
{
	// try 1...N permutation before proceeding, because it's often a solution
	std::iota(p.begin(), p.end(), 1);

	igraph.neighbors = prepareNeighbors();
	bool exists = igraph.createLGraph();
	bool next = true;

	// solution already found
	if (exists)
		return p;

	// look for viable permutation here
	for (size_t i = 1; i <= n; ++i)
	{
		// the first vertex is not permutred using functions below and all choices must be tried out
		p[0] = i;
		// loops through all viable permutations
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

	// all possibilities were tried without success, return empty
	p.clear();
	return p;
}

// sets first permutation for permutation with starting vertex v
// n n-1 ... v+1 v-1 ... 1 --> smallest possible permutation reversed
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

	// loops through all larger permutations looking for solution
	return newPermutation(usable);
}

// recursively loops through permutations looking for solution
// the permutations get larger and larger, until either solution is found or larger permutation does not exist
// the input parameter are the yet unset vertices in permutation p
// permutation variable p: v1 v2 v3 _ _ _ and usable: {v4, v5, v6}
bool Permutation::newPermutation(std::vector<size_t>& usable)
{
	size_t m = usable.size();

	// all positions are set, check if solution for the permutation exists
	if (m == 1)
	{
		p[n - 1] = usable[0];
		// test permutation here
		igraph.neighbors = prepareNeighbors();
		return igraph.createLGraph();
	}
	else
	{
		// position in usable that must be set before usable[m-1] or -1
		int pos = checkMoveValidity(usable, usable[m - 1]);

		// valid to place usable[m-1] at next position in p
		if (pos == -1)
		{
			p[n - m] = usable[m - 1];
			usable.pop_back();
			// set the rest of positions
			if (newPermutation(usable))
				return true;
			else
			{
				// usable[m-1] was not good choice, reverse it
				usable.push_back(p[n - m]);
				
				// try next in usable
				for (int j = m - 2; j >= 0; --j)
				{
					// usable[j] is moved before usable[m-1]
					size_t v = usable[j];
					// cannot set smaller permutation
					if (v < usable[m - 1])
						return false;

					usable.erase(usable.begin() + j);
					usable.push_back(v);

					// search using fixed usable
					if (newPermutation(usable))
						return true;
					// revert swap
					usable.pop_back();
					usable.insert(usable.begin() + j, v);
				}
				return false;
			}	
		}
		// remove collision by fixing 
		else
		{
			size_t temp = usable[m - 1];
			// cannot set smaller permutation
			if (temp > usable[m - 2])
				return false;

			// remove usable[m-1] and place it after pos
			usable.pop_back();
			usable.insert(usable.begin() + pos, temp);
			// search using fixed usable
			if (newPermutation(usable))
				return true;

			// try all places after pos for the vertex
			for (int j = pos - 1; j >= 0; --j)
			{
				// switch usable[pos] and usable[pos-1]
				size_t v = usable[j + 1];
				usable[j + 1] = usable[j];
				usable[j] = v;

				// search using new usable
				if (newPermutation(usable))
					return true;
			}

			// revert the initial swap
			usable.erase(usable.begin());
			usable.push_back(temp);
			return false;
		}
	}
	// not found
	return false;
}

// this function checks that by fixing vertex C in permutation already having A and B
// one would not find D such that AC and BD are edges and AB, BC and CD are not
// if this situation cannot happen -1 is returned
// else largest such D is returned
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

// set vertex IDs to match current permutation
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