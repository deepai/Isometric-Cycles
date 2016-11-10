#include "UF.h"

int UF::find(int r)
{
	if(parent[r] != r)
		parent[r] = find(r);
	else
		return r;
}

void UF::merge(int r1, int r2)
{
	int root1 = find(r1);
	int root2 = find(r2);

	if(root1 != root2)
	{
		if(rank[root1] >= rank[root2])
		{
			parent[root2] = root1;
			rank[root1] += rank[root2];
		}
		else
		{
			parent[root1] = root2;
			rank[root2] += rank[root1]; 
		}
	}
}