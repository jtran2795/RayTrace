#pragma once
// Note: you can put kd-tree heree
#include "scene.h"
class kdTree {
//Geometry* se;
private:
	kdTree* left;
	kdTree* right;
public:
	BoundingBox Boundary;
	Scene* scene;
	std::vector<glm::dvec3> coordinates;
	double dividing_plane;
	int dividing_dim;

	kdTree();
	kdTree(Scene* s, std::vector<Geometry*> objects, BoundingBox bb, int depth);
	intersect(const ray& r);
	//kdTree() {scene = s;}
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

