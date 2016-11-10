#ifndef __UNION_FIND_H
#define __UNION_FIND_H
struct UF
{
	int num_elements;
	int *rank;
	int *parent;

	UF(int n):num_elements(n)
	{
		rank = new int[num_elements];
		parent = new int[num_elements];

		for(int i=0;i<num_elements;i++)
		{
			parent[i] = i;
			rank[i] = 1;
		}
	}

	~UF()
	{
		delete[] rank;
		delete[] parent;
	}

	void merge(int r1, int r2);
	int find(int r);

};
#endif