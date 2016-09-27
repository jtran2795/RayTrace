#include <cmath>
#include <float.h>
#include <algorithm>
#include <glm/gtx/extented_min_max.hpp>
#include <glm/gtx/io.hpp>
#include <assert.h>
#include <string.h>
#include "trimesh.h"
#include "../scene/trimeshTree.h"
#include "../ui/TraceUI.h"

#include <iostream> //For debugging, remove later!

extern TraceUI* traceUI;

using namespace std;

Trimesh::~Trimesh()
{
	for( Materials::iterator i = materials.begin(); i != materials.end(); ++i )
		delete *i;
}

// must add vertices, normals, and materials IN ORDER
void Trimesh::addVertex( const glm::dvec3 &v )
{
    vertices.push_back( v );
}

void Trimesh::addMaterial( Material *m )
{
    materials.push_back( m );
}

void Trimesh::addNormal( const glm::dvec3 &n )
{
    normals.push_back( n );
}

// Returns false if the vertices a,b,c don't all exist
bool Trimesh::addFace( int a, int b, int c )
{
    int vcnt = vertices.size();

    if( a >= vcnt || b >= vcnt || c >= vcnt ) return false;

    TrimeshFace *newFace = new TrimeshFace( scene, new Material(*this->material), this, a, b, c );
    newFace->setTransform(this->transform);
    if (!newFace->degen) faces.push_back( newFace );


    // Don't add faces to the scene's object list so we can cull by bounding box
    // scene->add(newFace);
    return true;
}

const char* Trimesh::doubleCheck()
// Check to make sure that if we have per-vertex materials or normals
// they are the right number.
{
    if( !materials.empty() && materials.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of materials.";
    generateNormals();
    if( !normals.empty() && normals.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of normals.";
    tritree = tritree -> buildTree(faces, 15);
    return 0;
}

bool Trimesh::intersectLocal(ray& r, isect& i) const
{
	typedef Faces::const_iterator iter;
	bool have_one = false;
	/*for( iter j = faces.begin(); j != faces.end(); ++j )
	{
		isect cur;
		if( (*j)->intersectLocal( r, cur ) )
		{
			if( !have_one || (cur.t < i.t) )
			{
				i = cur;
				have_one = true;
			}
		}
	}
	if( !have_one ) i.setT(1000.0);
	return have_one;*/
    double tmin = 1000.0;
        //std::cout << "intersecting stuff again\n";
    //typedef vector<Geometry*>::const_iterator iter;
    if(!(tritree -> intersect(r,i,tritree,tmin)))
    {
        i.setT(1000.0);
        //std::cout << "Bad! \n";
    }
    else
    {
        if(i.t == 1000 || (i.N[0] == 0 && i.N[1] == 0 && i.N[2] == 0))
        {
            /*for(iter j = objects.begin(); j != objects.end(); ++j) {
                isect cur;
                if( (*j)->intersect(r, cur) ) {
                    if(!have_one || (cur.t < i.t)) {
                        i = cur;
                        have_one = true;
                    }
                }
            }
        if(!have_one) i.setT(1000.0);
            return have_one;*/
        //std::cout << "got wrong stuff   t" << i.t << " normal " << i.N << "\n";
        return false;
        }
        //std::cout << "got some stuff   t" << i.t << " normal " << i.N << "\n";
        return true;
    }
    return have_one;
} 

bool TrimeshFace::intersect(ray& r, isect& i) const {
  return intersectLocal(r, i);
}

// Intersect ray r with the triangle abc.  If it hits returns true,
// and put the parameter in t and the barycentric coordinates of the
// intersection in u (alpha) and v (beta).
bool TrimeshFace::intersectLocal(ray& r, isect& i) const
{
    // YOUR CODE HERE
    // 
    // FIXME: Add ray-trimesh intersection
    glm::dvec3 p1 = r.getPosition();
    glm::dvec3 d = r.getDirection();

    glm::dvec3 u = parent->vertices[ids[0]] - parent->vertices[ids[1]];
    glm::dvec3 v = parent->vertices[ids[1]] - parent->vertices[ids[2]];
    //glm::dvec3 N2 = normal;
    //cout << "A : "<< parent->vertices[ids[0]][0] << " " << parent->vertices[ids[0]][1] << " " << parent->vertices[ids[0]][2] << endl;
    //cout << "B : "<< parent->vertices[ids[1]][0] << " " << parent->vertices[ids[1]][1] << " " << parent->vertices[ids[1]][2] << endl;
    //cout << "C : "<< parent->vertices[ids[2]][0] << " " << parent->vertices[ids[2]][1] << " " << parent->vertices[ids[2]][2] << endl;
    //cout << "TRIMESH VECTOR : "<< N[0] << " " << N[1] << " " << N[2] << endl;

    double t = (glm::dot((parent -> vertices[ids[0]]),normal) - glm::dot(p1,normal))/glm::dot(d,normal);  

    if( t <= RAY_EPSILON ) {
        return false;
    }

    p1 = r.at(t);


    //cout << "t : " << t ;<< " ||| p : "<< p1[0] << " " << p1[1] << " " << p1[2] << endl;

    double max = 0;
    int maxindex = 0; //dimension to eliminate;
    for(int i = 0; i < 3; i++)
    {
        double temp3 = std::abs(normal[i]);
        if (temp3 > max)
        {
            max = temp3;
            maxindex = i;
        }
    }

    glm::dvec3 A = {0,0,0};
    glm::dvec3 B = {0,0,0};
    glm::dvec3 C = {0,0,0};
    glm::dvec3 p2;

    if(maxindex == 0)
    {
        //cout << "elimate X" << endl;
        A = {1,parent->vertices[ids[0]][1],parent->vertices[ids[0]][2]};
        B = {1,parent->vertices[ids[1]][1],parent->vertices[ids[1]][2]};
        C = {1,parent->vertices[ids[2]][1],parent->vertices[ids[2]][2]};
        p2 = {1,p1[1],p1[2]};
    }
    if(maxindex == 1)
    {
        //cout << "elimate Y" << endl;
        A = {parent->vertices[ids[0]][0],1,parent->vertices[ids[0]][2]};
        B = {parent->vertices[ids[1]][0],1,parent->vertices[ids[1]][2]};
        C = {parent->vertices[ids[2]][0],1,parent->vertices[ids[2]][2]};
        p2 = {p1[0],1,p1[2]};
    }
    if(maxindex == 2)
    {
        //cout << "elimate Z" << endl;
        A = {parent->vertices[ids[0]][0],parent->vertices[ids[0]][1],1};
        B = {parent->vertices[ids[1]][0],parent->vertices[ids[1]][1],1};
        C = {parent->vertices[ids[2]][0],parent->vertices[ids[2]][1],1};
        p2 = {p1[0],p1[1],1};
    }

    double ABCArea = A[0]*B[1]*C[2] + B[0]*C[1]*A[2] + C[0]*A[1]*B[2]
                   - B[0]*A[1]*C[2] - A[0]*C[1]*B[2] - C[0]*B[1]*A[2];

    double alpha = (p2[0]*B[1]*C[2] + B[0]*C[1]*p2[2] + C[0]*p2[1]*B[2]
                   - B[0]*p2[1]*C[2] - p2[0]*C[1]*B[2] - C[0]*B[1]*p2[2])/ABCArea;
    //cout << "ALPHA :" << alpha << endl;
    if (alpha < 0) return false;
    double beta = (A[0]*p2[1]*C[2] + p2[0]*C[1]*A[2] + C[0]*A[1]*p2[2]
                   - p2[0]*A[1]*C[2] - A[0]*C[1]*p2[2] - C[0]*p2[1]*A[2])/ABCArea;
    //cout << "BETA :" << beta << endl;
    if (beta < 0) return false;
    double gamma = (A[0]*B[1]*p2[2] + B[0]*p2[1]*A[2] + p2[0]*A[1]*B[2]
                   - B[0]*A[1]*p2[2] - A[0]*p2[1]*B[2] - p2[0]*B[1]*A[2])/ABCArea;
    //cout << "GAMMA :" << gamma << endl;
    if (gamma < 0) return false;

    glm::dvec3 temp1 = glm::cross(u,v);
    double temp2 = temp1[0]*temp1[0]+temp1[1]*temp1[1]+temp1[2]*temp1[2];
   // N

    //cout << "HIT!\n";
    i.obj = this;
    i.setMaterial(this->getMaterial());
    i.t = t;
    if(parent -> vertNorms){
    double N_x = glm::normalize(parent -> normals[ids[0]])[0]*alpha + glm::normalize(parent -> normals[ids[1]])[0]*beta + glm::normalize(parent -> normals[ids[2]])[0] *gamma;
    double N_y = glm::normalize(parent -> normals[ids[0]])[1]*alpha + glm::normalize(parent -> normals[ids[1]])[1]*beta + glm::normalize(parent -> normals[ids[2]])[1] *gamma;
    double N_z = glm::normalize(parent -> normals[ids[0]])[2]*alpha + glm::normalize(parent -> normals[ids[1]])[2]*beta + glm::normalize(parent -> normals[ids[2]])[2] *gamma;
    i.N = glm::normalize(glm::dvec3{N_x,N_y,N_z});
    }//glm::normalize(glm::dvec3{N_x,N_y,N_z});//normal;
    i.N = normal;
    //std::cout << "A B C" << alpha << " " << beta << " " << gamma << endl;
    //std::cout << "TRIMESH NORMALS" << parent ->  normals[ids[0]][0] << " " << parent ->  normals[ids[1]][0] << " " << parent ->  normals[ids[2]][0] << endl;
    //std::cout << "TRIMESH NORMALS" << N_x << " " << N_y << " " << N_z << endl;
    //abort();
    i.uvCoordinates = {alpha,beta};
    i.bary = {alpha,beta,gamma};
    return true;
}

void Trimesh::generateNormals()
// Once you've loaded all the verts and faces, we can generate per
// vertex normals by averaging the normals of the neighboring faces.
{
    int cnt = vertices.size();
    normals.resize( cnt );
    int *numFaces = new int[ cnt ]; // the number of faces assoc. with each vertex
    memset( numFaces, 0, sizeof(int)*cnt );
    
    for( Faces::iterator fi = faces.begin(); fi != faces.end(); ++fi )
    {
		glm::dvec3 faceNormal = (**fi).getNormal();
        
        for( int i = 0; i < 3; ++i )
        {
            normals[(**fi)[i]] += faceNormal;
            ++numFaces[(**fi)[i]];
        }
    }

    for( int i = 0; i < cnt; ++i )
    {
        if( numFaces[i] )
            normals[i]  /= numFaces[i];
    }

    delete [] numFaces;
    vertNorms = true;
}

