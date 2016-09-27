// The main ray tracer.

#pragma warning (disable: 4786)

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
//#include "scene/kdTree.h"

#include "parser/Tokenizer.h"
#include "parser/Parser.h"

#include "ui/TraceUI.h"
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>
#include <string.h> // for memset

#include <iostream>
#include <fstream>

using namespace std;
extern TraceUI* traceUI;

// Use this variable to decide if you want to print out
// debugging messages.  Gets set in the "trace single ray" mode
// in TraceGLWindow, for example.
bool debugMode = false;

// Trace a top-level ray through pixel(i,j), i.e. normalized window coordinates (x,y),
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.

glm::dvec3 RayTracer::trace(double x, double y, unsigned char *pixel, unsigned int ctr)
{
    // Clear out the ray cache in the scene for debugging purposes,
  if (TraceUI::m_debug) scene->intersectCache.clear();

    ray r(glm::dvec3(0,0,0), glm::dvec3(0,0,0), pixel, ctr, glm::dvec3(1,1,1), ray::VISIBILITY);
    scene->getCamera().rayThrough(x,y,r);
    double dummy;
    glm::dvec3 ret = traceRay(r, glm::dvec3(1.0,1.0,1.0), traceUI->getDepth() , dummy);
    ret = glm::clamp(ret, 0.0, 1.0);
    return ret;
}

glm::dvec3 RayTracer::tracePixel(int i, int j, unsigned int ctr)
{
	glm::dvec3 col(0,0,0);

	if( ! sceneLoaded() ) return col;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	//std::cout<< "-=-=-=" << ( i + (j) * buffer_width ) * 3 << std::endl;
	//std::cout << "-=0=0=" << ( i + (j) * buffer_width ) * 3 << std::endl;

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;
	col = trace(x, y, pixel, ctr);

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
	return col;
}


// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
glm::dvec3 RayTracer::traceRay(ray& r, const glm::dvec3& thresh, int depth, double& t )
{
	isect i;
	glm::dvec3 colorC;
	double n_i;
	double n_t;

	if(scene->intersect(r, i)) {
		//std::cout << "WE'RE DRAWING STFF NOW\n";
		// YOUR CODE HERE

		// An intersection occurred!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.
		if(depth < 0)
		{
			if (haveCubeMap()){
				return cubemap -> getColor(r);
			}
			return glm::dvec3(0.0, 0.0, 0.0);
		}
		//Get t
		double c_t = i.t;
		//Get N
		glm::dvec3 N2 = glm::normalize(i.N); 
		//Get mtrl
		const Material& m = i.getMaterial();

		//Evaluate to get q
		glm::dvec3 q = r.at(c_t);
		//Shade

		colorC = m.shade(scene, r, i);
		//Get Reflection Direction
		glm::dvec3 c_R = glm::reflect(N2,r.getDirection());
		//traceray color of the reflection vector, subtract depth
		//;
		if(m.kr(i)[0] > 0 || m.kr(i)[1] > 0 || m.kr(i)[2] > 0 )
		{
			ray r_2 = ray (q, glm::normalize(glm::reflect(r.getDirection(),N2)), 0, 0, glm::dvec3(1,1,1), ray::REFLECTION);
			colorC = colorC + m.kr(i)*traceRay(r_2, thresh, depth-1, t);
		}
		//cout << "MATERIAL k_r: " << m.kr(i)[0] << " " << m.kr(i)[1] << " " << m.kr(i)[2] << endl;
		//if entering
		if(!(m.kt(i)[0] > 0 || m.kt(i)[1] > 0 || m.kt(i)[2] > 0))
		{
			return colorC;
		}
		double entering = glm::dot(glm::normalize(r.getDirection()),glm::normalize(N2));
		if( entering < 0)
		//-- set indexes air = n_i and mtrl = n_t
		{

			//cout << "REFRACTION ENTERING " << 1.0 << " " << m.index(i) << endl;
			n_i = 1.0;
			n_t = m.index(i);
		}
		else if (entering > 0)
		{
			N2 = -N2;
			//cout << "REFRACTION LEAVING "  << m.index(i) << " " << 1.0 << endl;
			n_i = m.index(i);
			n_t = 1.0;
		}
		else
		{
			//cout << "PARALLEL" << endl;
			return glm::dvec3(0.0, 0.0, 0.0);
		}
		//cout << "---> Color " << colorC[0] << " " << colorC[1] << " " << colorC[2] << endl;
		//-- set indexes mtrl = n_t and mtrl = n_i

		//glm::dvec3 rf_angle = glm::normalize(glm::refract(r.getDirection(),N2,n_i/n_t));
		
		double tir = 1 - (glm::pow(n_i/n_t,2) * (1-glm::pow(glm::dot(N2,r.getDirection()),2)));
		//cout <<  "TIR value " << tir << endl;
		if((m.kt(i)[0] > 0 || m.kt(i)[1] > 0 || m.kt(i)[2] > 0) && !((tir < 0) && (n_t < n_i)))
		{
			
			ray r_3 = ray (q, glm::normalize(glm::refract(r.getDirection(),N2,n_i/n_t)), 0, 0, glm::dvec3(0,0,0), ray::REFRACTION);
			//cout << "REFRACTION HAPPENING " << r_3.getDirection()[0] << " " << r_3.getDirection()[1] << " " << r_3.getDirection()[2] << endl;
			colorC = colorC + m.kt(i)*traceRay(r_3, thresh, depth-1, t);
			//cout << "---> Color " << colorC[0] << " " << colorC[1] << " " << colorC[2] << endl;
		}
		//Color = Color + mtrl.k_t *traceray(Q,Translated)



		// Set T to refract directio
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.
		// 
		// FIXME: Add CubeMap support here.
			if (haveCubeMap()){
				return cubemap -> getColor(r);
			}
		colorC = glm::dvec3(0.0, 0.0, 0.0);
	}
	return colorC;
}

RayTracer::RayTracer()
	: scene(0), buffer(0), thresh(0), buffer_width(256), buffer_height(256), m_bBufferReady(false), cubemap (0)
{
}

RayTracer::~RayTracer()
{
	delete scene;
	delete [] buffer;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return sceneLoaded() ? scene->getCamera().getAspectRatio() : 1;
}

bool RayTracer::loadScene( char* fn ) {
	ifstream ifs( fn );
	if( !ifs ) {
		string msg( "Error: couldn't read scene file " );
		msg.append( fn );
		traceUI->alert( msg );
		return false;
	}
	
	// Strip off filename, leaving only the path:
	string path( fn );
	if( path.find_last_of( "\\/" ) == string::npos ) path = ".";
	else path = path.substr(0, path.find_last_of( "\\/" ));

	// Call this with 'true' for debug output from the tokenizer
	Tokenizer tokenizer( ifs, false );
	Parser parser( tokenizer, path );
	try {
		delete scene;
		scene = 0;
		scene = parser.parseScene();
	} 
	catch( SyntaxErrorException& pe ) {
		traceUI->alert( pe.formattedMessage() );
		return false;
	}
	catch( ParserException& pe ) {
		string msg( "Parser: fatal exception " );
		msg.append( pe.message() );
		traceUI->alert( msg );
		return false;
	}
	catch( TextureMapException e ) {
		string msg( "Texture mapping exception: " );
		msg.append( e.message() );
		traceUI->alert( msg );
		return false;
	}

	if( !sceneLoaded() ) return false;
	scene -> setupKd(traceUI -> kdSwitch(),traceUI -> getMaxDepth());
	return true;
}

void RayTracer::traceSetup(int w, int h)
{
	if (buffer_width != w || buffer_height != h)
	{
		buffer_width = w;
		buffer_height = h;
		bufferSize = buffer_width * buffer_height * 3;
		delete[] buffer;
		buffer = new unsigned char[bufferSize];
	}
	memset(buffer, 0, w*h*3);
	m_bBufferReady = true;
}

void RayTracer::traceImage(int w, int h, int bs, double thresh)
{
	//h = (w / aspectRatio() + 0.5);
	traceSetup( w, h );

	for( int j = 0; j < h; ++j )
		for( int i = 0; i < w; ++i )
			tracePixel(i,j,0);


	// YOUR CODE HERE
	// FIXME: Start one or more threads for ray tracing
}

int RayTracer::aaImage(int samples, double aaThresh)
{

	int real_height = (buffer_width / aspectRatio() + 0.5);
	for( int j = 0; j < buffer_height; ++j )
	{
		for( int i = 0; i < buffer_width; ++i )
			{
				double x = double(i)/double(buffer_width);
				double y = double(j)/double(buffer_height);
				bool aa_check = false;

				//cout << j << " "  << i << "" << ctr << endl;

				unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;
				//col = trace(x, y, pixel, ctr);

				if( i - 1 >= 0)
				{
					unsigned char *temp_pixel = buffer + ( i-1 + j * buffer_width ) * 3;
					if(checkNeighbors(pixel,temp_pixel, aaThresh))
					{
						aa_check = true;
						goto aa;
					}  
				}
				if( i + 1 < buffer_width)
				{
					unsigned char *temp_pixel = buffer + ( i+1 + j * buffer_width ) * 3;
					if(checkNeighbors(pixel,temp_pixel, aaThresh))
					{
						aa_check = true;
						goto aa;
					}  
				}
				if( j - 1 >= 0)
				{
					unsigned char *temp_pixel = buffer + ( i + (j-1) * buffer_width ) * 3;
					//std::cout<< "-=-=-=" << ( i + (j-1) * buffer_width ) * 3 << std::endl;
					//std::cout << "-=-=-=" << ( i + (j) * buffer_width ) * 3 << std::endl;
					//abort();
					if(checkNeighbors(pixel,temp_pixel, aaThresh))
					{
						aa_check = true;
						goto aa;
						
					}  
				}
				if( j + 1 < buffer_height)
				{
					unsigned char *temp_pixel = buffer + ( i + (j+1) * buffer_width ) * 3;
					//std::cout<< "-=-=-2" << ( i + (j-1) * buffer_width ) * 3 << std::endl;
					//std::cout << "-=-=-2" << ( i + (j) * buffer_width ) * 3 << std::endl;
					if(checkNeighbors(pixel,temp_pixel, aaThresh))
					{
						aa_check = true;
						goto aa;
					}  
				}
				if( i - 1 >= 0 &&  j - 1 >= 0)
				{
					unsigned char *temp_pixel = buffer + ( i-1 + (j-1) * buffer_width ) * 3;
					if(checkNeighbors(pixel,temp_pixel, aaThresh))
					{
						aa_check = true;
						goto aa;
					}  
				}
				if( i + 1 < buffer_width &&  j + 1 < buffer_height)
				{
					unsigned char *temp_pixel = buffer + ( i+1 + (j+1) * buffer_width ) * 3;
					if(checkNeighbors(pixel,temp_pixel, aaThresh))
					{
						aa_check = true;
						goto aa;
					}  
				}
				if( i + 1 < buffer_width &&  j - 1 >= 0)
				{
					unsigned char *temp_pixel = buffer + ( (i+1) + (j-1) * buffer_width ) * 3;
					if(checkNeighbors(pixel,temp_pixel, aaThresh))
					{
						aa_check = true;
						goto aa;
					}  
				}
				if( i - 1 >= 0 && j + 1 < buffer_height)
				{
					unsigned char *temp_pixel = buffer + ( (i-1) + (j+1) * buffer_width ) * 3;
					if(checkNeighbors(pixel,temp_pixel, aaThresh))
					{
						aa_check = true;
						goto aa;
					}  
				}
				aa:
				if(aa_check)
				{
					glm::dvec3 col = {0.0,0.0,0.0};
					double x_length = double(i+1)/double(buffer_width) - x;
					double y_length = double(j+1)/double(buffer_height) - y;
					//std::cout << "x " << x << " y " << y << std::endl;
					//std::cout << "xlength " << x_length << " ylength" << y_length << std::endl;
					//std::cout << y_length << endl;
					double x_offset = -x_length*(1/2) + x_length*(1/(samples*2));
					double y_offset = -y_length*(1/2) + y_length*(1/(samples*2));
					for(int k = 0; k < samples; k++)
					{
						for(int l = 0; l < samples; l++)
						{
							col += (trace((x+x_offset)+(k*x_length/samples), (y+y_offset)+(l*y_length/samples), pixel, 0))/(double(samples*samples));

							//std::cout << "AA CALC = " << ((x-x_offset)+(k*x_length/samples)) << " " << ((y-y_offset)+(l*y_length/samples)) << endl;
						}
					}
					//col /= samples*samples;
					//std::cout << "*** AA Color Final = " << col[0] << " " << col[1] << " " << col[2] << std::endl;
					pixel[0] = (int)( 255.0 * col[0]);
					pixel[1] = (int)( 255.0 * col[1]);
					pixel[2] = (int)( 255.0 * col[2]);
				}
				//abort();
			}
	}
	//std::cout << "Finished anti aliasing " << buffer_height << " " << real << endl;

}

bool RayTracer::checkNeighbors(unsigned char* pixel, unsigned char* neighbor, double aaThresh)
{
	double a = glm::abs(pixel[0]-neighbor[0])/255.0;
	double b = glm::abs(pixel[1]-neighbor[1])/255.0;
	double c = glm::abs(pixel[2]-neighbor[2])/255.0;
	if (a >= aaThresh || b >= aaThresh || c >= aaThresh)
	{
		return true;	
	}
	//std::cout << "bad\n";
	return false;
}

bool RayTracer::checkRender()
{
	// YOUR CODE HERE
	//std::cout << "DONE" << endl;

	// FIXME: Return true if tracing is done.
	return true;
}

glm::dvec3 RayTracer::getPixel(int i, int j)
{
	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;
	return glm::dvec3((double)pixel[0]/255.0, (double)pixel[1]/255.0, (double)pixel[2]/255.0);
}

void RayTracer::setPixel(int i, int j, glm::dvec3 color)
{
	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * color[0]);
	pixel[1] = (int)( 255.0 * color[1]);
	pixel[2] = (int)( 255.0 * color[2]);
}

