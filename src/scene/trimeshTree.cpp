// Note: you can put kd-tree heree
#include "trimeshTree.h"
#include <iostream>
trimeshTree::trimeshTree(){

}

trimeshTree* trimeshTree::buildTree(std::vector<TrimeshFace*> objs, int depth){

	std::vector<TrimeshFace*> left_list;
	std::vector<TrimeshFace*> right_list;

	trimeshTree *dNode = new trimeshTree();
	dNode -> left =  new trimeshTree();
	dNode -> right =  new trimeshTree();
	dNode -> boundary = BoundingBox();
	dNode -> obj_list = objs;

	//std::cout << "Building Tree" << std::endl;

	if(objs.size() == 0)
	{
		return dNode;
	}
	if(objs.size() == 1)
	{
		dNode -> left = new trimeshTree();
		dNode -> left -> obj_list = std::vector<TrimeshFace*>();
		dNode -> right = new trimeshTree();
		dNode -> right -> obj_list = std::vector<TrimeshFace*>();
		dNode -> boundary =  objs[0] -> getBoundingBox();
		//dNode -> obj_list = objs;
		return dNode;
	}

	

	dNode -> boundary = objs[0] -> getBoundingBox();
	for(int i = 1; i < objs.size(); i++) 
	{
		dNode -> boundary.merge(objs[i] -> getBoundingBox());
	}

	/*if(depth == 0)
	{
		dNode -> left = new trimeshTree();
		dNode -> left -> obj_list = std::vector<TrimeshFace*>();
		dNode -> right = new trimeshTree();
		dNode -> right -> obj_list = std::vector<TrimeshFace*>();
		return dNode;
	}*/
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

//splitting via sah (takes too much time to build !!!)
/*
	double best_plane = 0;
	double best_axis = -1;
	double best_cost = 100e10;
	int    best_m = -1;
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < objs.size(); j++)
		{
			for(int m = 0; m < 2; m++)
			{
				double div_plane;
				if(m == 0) div_plane = (objs[j]->getBoundingBox()).getMin()[i];
				if(m == 1) div_plane = (objs[j]->getBoundingBox()).getMax()[i];
				BoundingBox left_box = BoundingBox();
				BoundingBox right_box = BoundingBox();
				
				int n_l = 0;
				int n_r = 0;
				for(int k = 0; k < objs.size(); k++)
				{
					if(m == 0)
					{
						if((objs[k]->getBoundingBox()).getMax()[i] <= div_plane)
						{
							n_l++;
							left_box.merge(objs[k] -> getBoundingBox());
						}
						else
						{
							n_r++;
							right_box.merge(objs[k] -> getBoundingBox());
						}
					}
					if(m == 1)
					{
						if((objs[k]->getBoundingBox()).getMin()[i] > div_plane)
						{
							n_l++;
							left_box.merge(objs[k] -> getBoundingBox());
						}
						else
						{
							n_r++;
							right_box.merge(objs[k] -> getBoundingBox());
						}
					}
				}
				double sa_l = left_box.area();
				double sa_r = right_box.area();

				double sa_hue = sa_l*n_l + sa_r*n_r;
				if(sa_hue < best_cost)
				{
					best_cost = sa_hue;
					best_axis = i;
					best_plane = div_plane;
					best_m = m;
				}
			}
		}
	}

	for(int i = 0; i < objs.size(); i++)
	{
		if(best_m == 0)
		{
			if((objs[i]->getBoundingBox()).getMax()[best_axis] <= best_plane)
			{
				left_list.push_back(objs[i]);
			}
			else
			{
				right_list.push_back(objs[i]);
			}
		}
		if(best_m == 1)
		{
			if((objs[i]->getBoundingBox()).getMin()[best_axis] > best_plane)
			{
				left_list.push_back(objs[i]);
			}
			else
			{
				right_list.push_back(objs[i]);
			}
		}
	}
*/

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
	else {
	 	dNode -> left = new trimeshTree();
	 	dNode -> left -> obj_list = std::vector<TrimeshFace*>();
	 	dNode -> right = new trimeshTree();
	 	dNode -> right -> obj_list = std::vector<TrimeshFace*>();
	 	return dNode;
	 }
	// }
	//return dNode;
}

bool trimeshTree::intersect( ray& r, isect& i, trimeshTree* dNode, double& smallest)
{
	//double t_min;
	//double t_max;

//std::cout << "about to intersect \n";
	if(dNode -> boundary.intersect(r))
	{
		glm::dvec3 normal = {0,0,0};


		//glm::dvec3 p_global;
		//glm::dvec3 p;

		isect old;
		old.t = -1;
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
			bool left_path = intersect(r,i,dNode -> left, smallest);
			//std::cout << "Finsihed left \n";
			
			bool right_path = intersect(r,i_right,dNode -> right, smallest);
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
					smallest = i_right.t;
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
					if(!( i.t == 0 || i.t == 1000 || (i.N[0] == 0 && i.N[1] == 0 && i.N[2] == 0)))
					{
						intersected = true;
						if(i.t < smallest)
						{
							smallest = i.t;
							old = i;
						}
					}
				}
			}
			if(intersected)
			{
				//if(tMin != 1000.0){
				if(old.t != -1){
				i = old;}
				//i.obj = o;
				//i.uvCoordinates = uvCoordinates;
				//i.material = m;
				if(i.t == 0)
				{
					//std::cout << "False. Not actual shit.\n";
					return false;
				}
				//i.bary = bary;//}
				//std::cout << "Returned true. \n";
				return true;
				
			}
			//std::cout << "False, didn't intersect.\n";
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

