// Note: you can put kd-tree heree
#include "kdTree.h"
#include <iostream>
kdTree::kdTree(){}
kdTree::kdTree(std::vector<Geometry*> objects, BoundingBox bb, int depth) {
	Boundary = bb;
	scene = s;
	if (depth == 0)
	{
		std::cout << "BoundingBox Shape " << bb.getMin()[0] << " " << bb.getMin()[1] << " " << bb.getMin()[2] << " x "  << bb.getMax()[0] << " " << bb.getMax()[1] << " " << bb.getMax()[2] << std::endl;
		std::cout << "Amount of objects in it. " << objects.size() << std::endl;
		return;
	}
	for(std::vector<Geometry*>::const_iterator begin = objects.begin(); begin !=  objects.end(); begin++)
	{
		coordinates.push_back( (*begin) -> getBoundingBox().getMin() );
		coordinates.push_back( (*begin) -> getBoundingBox().getMax() );
	}

	//x axis;
	int axis_of_choice = 0;
	double maxcost = 1000000000;
	double maxcost_index = 0;
	double maxplane = 0;



	std::vector<Geometry*> left_list;
	std::vector<Geometry*> right_list;
	for(int dimension = 0; dimension < 3; dimension++)
	{
		for(std::vector<glm::dvec3>::iterator begin = coordinates.begin(); begin != coordinates.end(); begin++)
		{
			double plane = (*begin)[dimension];
			int left_cost = 0;
			int right_cost = 0;
			int n_a = 0;
			double s_a = 0;
			int n_b = 0;
			double s_b = 0;
			double s_total = 0;


			for(std::vector<Geometry*>::const_iterator begin2 = objects.begin(); begin2 !=  objects.end(); begin2++)
			{
				//check if they are on the left or right side
				if(((*begin2) -> getBoundingBox().getMax())[dimension] <= plane) //check left
				{
					n_a++;
					BoundingBox temp_box = (*begin2) -> getBoundingBox();
					s_a += temp_box.area();
					//near.push_back(*begin2);

				}
				if(((*begin2) -> getBoundingBox().getMin())[dimension] >  plane) //check right
				{
					n_b++;
					BoundingBox temp_box = (*begin2) -> getBoundingBox();
					s_b += temp_box.area();
										//far.push_back(*begin2);
				}
				BoundingBox temp_box = (*begin2) -> getBoundingBox();
				s_total += temp_box.area();
			}

			double cost = n_a*s_a + n_b*s_b;
			//std::cout << "Cost calculated " << cost << " at " << plane << " " << dimension << std::endl;    
			if(cost < maxcost)
			{
				maxcost = cost;
				maxcost_index = dimension;
				maxplane = plane;
			}
		}
		std::cout << dimension << std::endl;
	}
	//std::cout << "*** DECISION *** >> " << maxcost << " at " << maxplane << " " << maxcost_index <<  std::endl;
	for(std::vector<Geometry*>::const_iterator begin2 = objects.begin(); begin2 !=  objects.end(); begin2++)
	{
		//check if they are on the left or right side
		if(((*begin2) -> getBoundingBox().getMin())[maxcost_index] <= maxplane) //check left
		{
			left_list.push_back(*begin2);
		}
		if(((*begin2) -> getBoundingBox().getMax())[maxcost_index] >  maxplane) //check right
		{
			right_list.push_back(*begin2);
		}
	}
	dividing_plane = maxplane;
	dividing_dim   = maxcost_index;
	glm::dvec3 leftbound_min = bb.getMin();
	glm::dvec3 leftbound_max = bb.getMax();
	leftbound_max[maxcost_index] = maxplane;

	glm::dvec3 rightbound_min = bb.getMin();
	glm::dvec3 rightbound_max = bb.getMax();
	rightbound_min[maxcost_index] = maxplane;

	BoundingBox leftBound = BoundingBox(leftbound_min,leftbound_max);
	BoundingBox rightBound = BoundingBox(rightbound_min,rightbound_max);

	left = new kdTree{s, left_list, leftBound, depth - 1};
	right = new kdTree{s, right_list, rightBound, depth - 1};
}

bool kdTree::intersect(const ray& r, isect& i, std::vector<Geometry*>& rlist;)
	{
		double t_min;
		double t_max;
		bool intersected = Boundary.intersect(r,t_min,t_max);
		
		glm::dvec3 p = r.getPosition();
		glm::dvec3 d = r.getDirection();
		double t_star = -p[dividing_dim]/d[dividing_dim];

		if(left == NULL && right == NULL)
		{
			bool intersection;
			
			for(std::vector<Geometry*>::const_iterator begin = obj_list.begin(); begin != obj_list.end(); begin++)
			{
				rlist.push_back((*begin));
			}
			return true;

		}
		if( t_star <= RAY_EPSILON ) {
			return false;
		}

		if(t_max < t_star)
		{
			left -> intersect(ray(r.at(t_min),r.getDirection(),0,0,glm::dvec3{0,0,0},ray::VISIBILITY),i);
		}

		if(t_min < t_star < t_max)
		{
			left -> intersect(ray(r.at(t_min),r.getDirection(),0,0,glm::dvec3{0,0,0},ray::VISIBILITY),i);
			right -> intersect(ray(r.at(t_star),r.getDirection(),0,0,glm::dvec3{0,0,0},ray::VISIBILITY),i);
		}
		if(t_star < t_min)
		{
			right -> intersect(ray(r.at(t_min),r.getDirection(),0,0,glm::dvec3{0,0,0},ray::VISIBILITY),i);
		}

		std::cout << "NOT EVEN." << std::endl;

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

