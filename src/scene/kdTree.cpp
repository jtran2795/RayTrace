// Note: you can put kd-tree heree
#include "kdTree.h"
#include <iostream>
kdTree::kdTree(){

}

kdTree* kdTree::buildTree(std::vector<Geometry*> objs, int depth){
	std::vector<Geometry*> left_list;
	std::vector<Geometry*> right_list;

	kdTree *dNode = new kdTree();
	dNode -> left = NULL;
	dNode -> right = NULL;
	dNode -> boundary = BoundingBox();
	dNode -> obj_list = objs;

	if(objs.size() == 0)
	{
		return dNode;
	}
	if(objs.size() == 1)
	{
		dNode -> left = new kdTree();
		dNode -> left -> obj_list = std::vector<Geometry*>();
		dNode -> right = new kdTree();
		dNode -> right -> obj_list = std::vector<Geometry*>();
		dNode -> boundary =  objs[0] -> getBoundingBox();
		//dNode -> obj_list = objs;
		return dNode;
	}
	if(depth == 0)
	{
		dNode -> left = new kdTree();
		dNode -> left -> obj_list = std::vector<Geometry*>();
		dNode -> right = new kdTree();
		dNode -> right -> obj_list = std::vector<Geometry*>();
		return dNode;
	}
	//get bbox of all geometry
	dNode -> boundary = objs[0] -> getBoundingBox();

	for(int i = 1; i < objs.size(); i++) 
	{
		dNode -> boundary.merge(objs[i] -> getBoundingBox());
	}

	glm::dvec3 mid;
	for(int i = 0; i < objs.size(); i++)
	{
		mid+= (objs[i] -> getBoundingBox().getMid() * (1.0/objs.size()) );
	}

	double largest_axis = 0;
	int axis_index = 0;
	for(int i = 0; i < 3; i++)
	{
		double temp = glm::abs((dNode -> boundary.getMax())[i] - (dNode -> boundary.getMin())[i]);
		if(temp > largest_axis)
		{
			largest_axis = temp;
			axis_index = i;
		}
	}

	for(int i=0; i < objs.size(); i++)
	{
		if(axis_index == 0)
		{
			if(mid[0] >= ((objs[i]->getBoundingBox()).getMid())[0])
			{
				right_list.push_back(objs[i]);
			}
			else
			{
				left_list.push_back(objs[i]);
			
			}
		}
		if(axis_index == 1)
		{
			if(mid[1] >= ((objs[i]->getBoundingBox()).getMid())[1])
			{
				right_list.push_back(objs[i]);
			}
			else
			{
				left_list.push_back(objs[i]);
			}
		}
		if(axis_index == 2)
		{
			if(mid[2] >= ((objs[i]->getBoundingBox()).getMid())[2])
			{
				right_list.push_back(objs[i]);
			}
			else
			{
				left_list.push_back(objs[i]);
			}
		}
	}

	if(left_list.size() == 0 && right_list.size() > 0)
	{
		left_list = right_list;
	}
	if(right_list.size() == 0 && left_list.size() > 0)
	{
		right_list = left_list;
	}

	double count = 0;
	for(int i = 0; i < left_list.size(); i++)
	{
		for(int j = 0; j < right_list.size(); j++)
		{
			if(left_list[i] == right_list[i])
			{
				count++;
			}
		}
	}

	if ((count/left_list.size() < 0.5) && (count/right_list.size() < 0.5))
	{
		dNode -> left = buildTree(left_list, depth - 1);
		dNode -> right = buildTree(right_list, depth - 1);
		return dNode;
	}
	else
	{
		dNode -> left = new kdTree();
		dNode -> left -> obj_list = std::vector<Geometry*>();
		dNode -> right = new kdTree();
		dNode -> right -> obj_list = std::vector<Geometry*>();
		return dNode;
	}
	return dNode;
}

bool kdTree::intersect( ray& r, isect& i, kdTree* dNode, double& tMin, double& tStar)
{
	double t_min;
	double t_max;

	if(dNode -> boundary.intersect(r))
	{
		glm::dvec3 normal = {0,0,0};


		glm::dvec3 p_global;
		glm::dvec3 p;

		isect old;
		// Material *m;
		// const SceneObject *o;
	 //    glm::dvec2 uvCoordinates;
	 //    glm::dvec3 bary; 


			
		bool intersected;
		if(dNode -> left -> obj_list.size() > 0 || dNode -> right -> obj_list.size() > 0 )
		{
			//isect i_left;
			//i_left.t = 1000.0;
			
			//i_right.t = 1000.0;
			//std::cout << "Entered \n";
			isect i_right = i;
			bool left_path = intersect(r,i,dNode -> left,tMin, tStar);
			//std::cout << "Finsihed left \n";
			
			bool right_path = intersect(r,i_right,dNode -> right,tMin, tStar);
			//std::cout << "Finsihed right \n";
			if(!left_path && right_path)
			{
				i = i_right;
			}
			if(left_path && right_path)
			{
				if(i_right.t < i.t)
				{
					i = i_right;
					tMin = i_right.t;
				}
			}
			return (left_path || right_path);
			//std::cout << "Finished both \n";
		}
		else{
			for(int j = 0; j < (dNode -> obj_list.size()); j++)
			{
				if((dNode -> obj_list)[j] -> intersect(r,i))
				{
					intersected = true;
					if(i.t < tMin)
					{
						tMin = i.t;
						old = i;
					}
				}
			}
			if(intersected)
			{
				//if(tMin != 1000.0){
				if(old.t != 0){
				i = old;}
				//i.obj = o;
				//i.uvCoordinates = uvCoordinates;
				//i.material = m;
				//i.bary = bary;//}
				//std::cout << "Finished looking through list \n";
				return true;
				
			}
			return false;
		}
	}
	else{
		//std::cout <<"missed bbox \n";
	}
	//std::cout << "Nothing hit \n";
	return false;
}

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

