#include "disjoint_set.h"

using namespace std;

DisjointSet::DisjointSet(int size)
{
    parents = std::vector<int>(size);
    sizes = std::vector<int>(size);
    for(int i = 0; i<size; i++){
        parents[i] = i;
        sizes[i] = 1;
    }
}

int	DisjointSet::get_set(int element)
{
    if(parents[element]==element)
        return element;
    return parents[element] = get_set(parents[element]);
}

int DisjointSet::count_differents() const
{
    int count = 0;
    for (int i = 0; i < parents.size(); i++) {
        if(parents[i]==i)
            count++;
    }
    return count;
}

int DisjointSet::get_set_size(int element)
{
    return sizes[get_set(element)];
}

int	DisjointSet::union_sets(int element0, int element1)
{
    element0 = get_set(element0);
    element1 = get_set(element1);
    if(element1==element0)
        return element0;
    if(sizes[element0]>sizes[element1]){
        parents[element1] = element0;
        sizes[element0]+= sizes[element1];
        return element0;
    } else {
        parents[element0] = element1;
        sizes[element1]+= sizes[element0];
        return element1;
    }
}
