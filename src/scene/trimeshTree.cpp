// Note: you can put kd-tree heree
#include "trimeshTree.h"
#include <iostream>
trimeshTree::trimeshTree(){

}

trimeshTree* trimeshTree::buildTree(std::vector<TrimeshFace*> objs, int depth){

	std::vector<TrimeshFace*> left_list;
	std::vector<TrimeshFace*> right_list;

	trimeshTree *dNode = new trimeshTree();
	dNode -> boundary = BoundingBox();
	dNode -> obj_list = objs;

	//std::cout << "Building Tree" << std::endl;

	dNode -> left =  new trimeshTree();
	dNode -> right =  new trimeshTree();

	if(objs.size() == 0) //no objects in node
	{
		return dNode;
	}

	if(objs.size() == 1) //one object in node
	{
		dNode -> left -> obj_list = std::vector<TrimeshFace*>();
		dNode -> right -> obj_list = std::vector<TrimeshFace*>();
		dNode -> boundary =  objs[0] -> getBoundingBox();
		return dNode;
	}

	for(int i = 0; i < objs.size(); i++) 
	{
		dNode -> boundary.merge(objs[i] -> getBoundingBox());
	}

	glm::dvec3 mid;
	for(int i = 0; i < objs.size(); i++)
	{
		mid+= ((objs[i] -> getBoundingBox().getMid()) * (1.0/objs.size()) );
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

//splitting via Surface Area Hueristic takes too long on Trimeshes.
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

//Split by midpoints.

	for(int i=0; i < objs.size(); i++) //add to left or right list depeding on location from midpoint
	{
		if(mid[axis_index] < ((objs[i]->getBoundingBox()).getMid())[axis_index])
		{
			left_list.push_back(objs[i]);
		}
		else
		{
			right_list.push_back(objs[i]);
		}
	}

	double count = 0; //check for same value in either lists
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
	double left_matches = count/left_list.size();
	double right_matches = count/right_list.size();
	double thresh = 0.5;
	if (left_matches < thresh && right_matches < thresh)
	{
		dNode -> left = buildTree(left_list, depth - 1);
		dNode -> right = buildTree(right_list, depth - 1);
		return dNode;
	}
	else 
	{
	 	dNode -> left = new trimeshTree();
	 	dNode -> left -> obj_list = std::vector<TrimeshFace*>();

	 	dNode -> right = new trimeshTree();
	 	dNode -> right -> obj_list = std::vector<TrimeshFace*>();
	 	return dNode;
	 }
}

//based on bounding box heirchy search, checks for intersections with bounding boxes
bool trimeshTree::intersect( ray& r, isect& i, trimeshTree* dNode, double& smallest)
{

//std::cout << "about to intersect \n";
	if(dNode -> boundary.intersect(r))
	{
		isect old; // used for later
		old.t = -1;
		bool intersected;
		if((dNode -> left -> obj_list.size() > 0) || (dNode -> right -> obj_list.size() > 0) ) // check if either lists are not empty
		{
			isect i_right = i; //don't overwrite i
			bool left_path = intersect(r,i,dNode -> left, smallest); //recurse
			bool right_path = intersect(r,i_right,dNode -> right, smallest); //recurse
			if(!left_path && right_path) //if no left list intersection, i is the right list
			{
				i = i_right;
				return true;
			}
			if(left_path && right_path) //check which intersection was closer
			{
				if(i_right.t < i.t)
				{
					i = i_right;
					smallest = i_right.t;
				}
				return true;
			}
			return (left_path || right_path); //return if there was intersection
		}
		else{
			for(int j = 0; j < (dNode -> obj_list.size()); j++) //check every geometry for intersection
			{
				if((dNode -> obj_list)[j] -> intersect(r,i))
				{
					if(!( i.t == 0 || i.t == 1000 || (i.N[0] == 0 && i.N[1] == 0 && i.N[2] == 0))) //invalid isect
					{
						intersected = true;
						if(i.t < smallest) //check if smallest
						{
							smallest = i.t;
							old = i;
						}
					}
				}
			}
			if(intersected) // there was an intersection
			{
				if(old.t != -1){ //there was an old value set already
				i = old;} //use it
				if(i.t == 0) //not really an intersection
				{
					//std::cout << "False.""
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
	//std::cout << "Nothing intersected \n";
	return false;
}
