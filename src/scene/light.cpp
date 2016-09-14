#include <cmath>

#include "light.h"
#include <glm/glm.hpp>

#include <iostream> //For debugging, remove later!
using namespace std;

double DirectionalLight::distanceAttenuation(const glm::dvec3& P) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


glm::dvec3 DirectionalLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	glm::dvec3 d = getDirection(p);
	std::cout << "DIRECTIONAL LIGHT" << endl;
	std::cout << "Directional of p "<< d[0] << " " << d[1] << " " << d[2] << endl;
	ray n_ray = ray(p,-d,0,0,glm::dvec3(1,1,1),ray::SHADOW);
	
    //from infinity to zero

	isect i;
	//scene -> 

	//double t = i.t;
	//glm::dvec3 q = r.at(t);
	//glm::dvec3 ld = (position-p);

	//std::cout << "Directional t"<< t << endl;
	//std::cout << "Length"<< sqrt(q[1]*q[1]+q[2]*q[2]+q[0]*q[0]) << endl;

	if(!(scene -> intersect(n_ray,i)))//sqrt(q[1]*q[1]+q[2]*q[2]+q[0]*q[0]) < 900)
	{
		std::cout << "No light Directional" << endl;
		return glm::dvec3(0,0,0);
		
	}
	else
	{
		std::cout << "Some light" << endl;
		return glm::dvec3(1,1,1);
	}
	return glm::dvec3(1.0, 1.0, 1.0);
}

glm::dvec3 DirectionalLight::getColor() const
{
	return color;
}

glm::dvec3 DirectionalLight::getDirection(const glm::dvec3& P) const
{
	return -orientation;
}

double PointLight::distanceAttenuation(const glm::dvec3& P) const
{

	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, we assume no attenuation and just return 1.0
	//glm::dvec3 ld = (position-P);
	//double result = ld[1]*ld[1]+ld[2]*ld[2]+ld[0]*ld[0];
	return 1;//result;
}

glm::dvec3 PointLight::getColor() const
{
	return color;
}

glm::dvec3 PointLight::getDirection(const glm::dvec3& P) const
{
	return glm::normalize(position - P);
}


glm::dvec3 PointLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	
	glm::dvec3 d = getDirection(p);
	std::cout << p[0] << " " << p[1] << " " << p[2] << endl;
	ray n_ray = ray(p,d,0,0,glm::dvec3(1,1,1),ray::SHADOW);
	isect i;
	if(!(scene -> intersect(n_ray,i)))
	{
		std::cout << "DEAD";
		return glm::dvec3(1,1,1);
	}
	//double t = i.t;
	glm::dvec3 q = n_ray.at(i.t);
	double qlength = glm::distance(p,q);
	double lightlength = glm::distance(p,position);

	std::cout << "qlength = " << qlength << " lightlength " << lightlength << endl;

	//std::cout << "q length :"<< sqrt(q[1]*q[1]+q[2]*q[2]+q[0]*q[0]) << "other length " << sqrt(ld[1]*ld[1]+ld[2]*ld[2]+ld[0]*ld[0]) << endl;
	if(qlength < lightlength)
	{
		std::cout << "No light" << endl;
		return glm::dvec3(0,0,0);
		
	}
	else
	{
		std::cout << "Some light" << endl;
		return glm::dvec3(1,1,1);
	}
}

