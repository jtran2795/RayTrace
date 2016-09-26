#pragma once
// Note: you can put kd-tree heree
//#include "scene.h"
#include "../SceneObjects/trimesh.h"
class TrimeshFace;
class trimeshTree {
//TrimeshFace* se;
private:
	trimeshTree* left;
	trimeshTree* right;
	BoundingBox boundary;
	std::vector<TrimeshFace*> obj_list;
public:
	trimeshTree();
	trimeshTree* buildTree(std::vector<TrimeshFace*> objs, int depth);
	bool intersect(ray& r, isect& i, trimeshTree* dNode, double& smallest);

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

