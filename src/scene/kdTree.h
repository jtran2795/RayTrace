#pragma once
// Note: you can put kd-tree heree
#include "scene.h"
class Geometry;
class kdTree {
//Geometry* se;
private:
	kdTree* left;
	kdTree* right;
	BoundingBox boundary;
	std::vector<Geometry*> obj_list;
public:
	kdTree();
	kdTree* buildTree(std::vector<Geometry*> objs, int depth);
	bool intersect(ray& r, isect& i, kdTree* dNode, double& tMin, double& tStar);

	//void setup();
	//void buildTree();

};

/*build tree

1. Check costs.
for(int i = 0; i < scene.objects, list of division planes)// the object list is sorted by x,y,z axis
{
	cost = Sa/S
	if (cost > maxcost)
	{
		maxcost = cost;
		maxcost_index = cost;
	}
}

That's the division
get objects on near and far side
set into the near and far node
pass through build tree with the new node and the list of objects
recurse
do the same with far side
recurse
done
*/

