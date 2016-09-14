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

    d[maxindex] = 2;
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
		}

	if(max < 0 && maxindex == 0) //neg x axis
	{
		currentmap = 1;
	}
	if(max > 0 && maxindex == 0) //pos x axis
	{
		currentmap = 0;
	}
	if(max < 0 && maxindex == 1) //neg y axis
	{
		currentmap = 3;
	}
	if(max > 0 && maxindex == 1) //neg y axis
	{
		currentmap = 2;
	}
	if(max < 0 && maxindex == 2) //neg z axis
	{
		currentmap = 4;
	}
	if(max > 0 && maxindex == 2) //neg z axis
	{
		currentmap = 5;
	}

	double x = 0;
	double y = 0;

	int savedindex;
	for(savedindex = 0; savedindex < 3; savedindex++)
	{
		if(coordinates[savedindex] == -1)
		{
			continue;
		}
		else
		{
			x = coordinates[savedindex];
			break;
		}
	}

	for(; savedindex < 3; savedindex++)
	{
		if(coordinates[savedindex] == -1)
		{
			continue;
		}
		else
		{
			y = coordinates[savedindex];
			break;
		}
	}

	double u = x/length;
	double v = y/length;

	std::cout << " CUBEMAP X " << x << " CUBEMAP Y " << y << std::endl;

	return tMap[currentmap] -> getMappedValue(glm::dvec2{u,v});
}
