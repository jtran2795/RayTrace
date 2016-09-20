#include "cubeMap.h"
#include "ray.h"
#include "../ui/TraceUI.h"
extern TraceUI* traceUI;

#include <iostream>

glm::dvec3 CubeMap::getColor(ray r) const {

	// YOUR CODE HERE
	// FIXME: Implement Cube Map here
	int currentmap = 0;
	ray maxray = ray(glm::dvec3{0,0,0},glm::dvec3{1,1,1},0,0,glm::dvec3(0,0,0),ray::VISIBILITY);
	glm::dvec3 maxcoordinates = maxray.at(500);
	double half = std::abs(maxcoordinates[0]);
	double length = half * 2; 

	glm::dvec3 d = glm::normalize(r.getDirection());
    double max = 0;
    int maxindex = 0; //dimension to eliminate;
    for(int i = 0; i < 3; i++)
    {
        double temp3 = std::abs(d[i]);
        if (temp3 > std::abs(max))
        {
            max = d[i];
            maxindex = i;
        }
    }

/*    d[maxindex] = 2;
    std::cout << "CUBEMAP d COORDS " << d[0] << " " << d[1] << " " << d[2] << std::endl;
    glm::dvec3 coordinates = {-1,-1,-1};
	for(int i = 0; i < 3; i++)
		{
			if(d[i] == 2) 
			{
				continue;
			}
			if(d[i] < 0)
			{
				coordinates[i] = half + half*d[0];
			}
			else
			{
				coordinates[i] = half + half*d[i];
			}
		}*/

	double sc = 0;
	double tc = 0;
	double ma = 0;

	if(max < 0 && maxindex == 0) //neg x axis
	{
		sc = d[2];
		tc = -d[1];
		ma = d[0];
		currentmap = 1;

	}
	if(max > 0 && maxindex == 0) //pos x axis
	{
		sc = -d[2];
		tc = -d[1];
		ma = d[0];
		currentmap = 0;
	}
	if(max < 0 && maxindex == 1) //neg y axis
	{
		sc = d[0];
		tc = -d[2];
		ma = d[1];
		currentmap = 3;
	}
	if(max > 0 && maxindex == 1) //neg y axis
	{
		sc = d[0];
		tc = d[2];
		ma = d[1];
		currentmap = 2;
	}
	if(max < 0 && maxindex == 2) //neg z axis
	{
		sc = -d[0];
		tc = -d[1];
		ma = d[2];
		currentmap = 4;
	}
	if(max > 0 && maxindex == 2) //neg z axis
	{	
		sc = d[0];
		tc = d[1];
		ma = d[2];
		currentmap = 5;
	}

	double x = (sc/glm::abs(ma) + 1) / 2;

	double y = (tc/glm::abs(ma) + 1) / 2;

	std::cout << " CUBEMAP X " << x << " CUBEMAP Y " << 1-y << std::endl;

	return tMap[currentmap] -> getMappedValue(glm::dvec2{x,1-y});
}
