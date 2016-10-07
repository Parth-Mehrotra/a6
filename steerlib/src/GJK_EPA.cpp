#include "obstacles/GJK_EPA.h"


SteerLib::GJK_EPA::GJK_EPA()
{
}

//GJK finish
//get support of points with certain direction
//input: list of vertices, count and dirtection
Util::Vector SteerLib::GJK_EPA::getSupport(std::vector<Util::Vector> vertices, int count, Util::Vector d) {
	float highest = -FLT_MAX;
	Util::Vector support(0, 0, 0);

	for (int i = 0; i < count; ++i) {
		Util::Vector v = vertices[i];
		float dot = v.x*d.x + v.z*d.z;
		if (dot > highest) {
			highest = dot;
			support = v;
		}
	}

	return support;

}
bool SteerLib::GJK_EPA::doSimplex(std::vector<Util::Vector> &list, Util::Vector &d) {
	//either line or triangle

	//line case
	if (list.size() == 2) {
		//std::cout << "print points: " << "Point B:"<< list[0] << "Point A:" << list[1] << std::endl;
		//AB->
		Util::Vector pointA = list[1];
		Util::Vector pointB = list[0];
		Util::Vector vectorAB = pointB - pointA;
		std::cout << "vectorAB:" << vectorAB << std::endl;
		Util::Vector vectorAO = -pointA;

		//if AB.A0>0
		if (vectorAB*vectorAO > 0) {
			list.clear();
			//then simplex is A,B
			list.push_back(pointB);
			list.push_back(pointA);
			
			//d=ABxA0xAB
			d = cross(cross(vectorAB, vectorAO), vectorAB);
			return false;
		}
		else {
			//then simplex is A
			list.clear();
			list.push_back(pointA);
			//d=AO
			d = vectorAO;
			return false;
		}
	}
	//triangle case
	
	else {
		Util::Vector pointA = list[2];
		Util::Vector pointB = list[1];
		Util::Vector pointC = list[0];
		Util::Vector vectorAB = pointB - pointA;
		Util::Vector vectorAC = pointC - pointA;
		Util::Vector vectorABC = cross(vectorAB,vectorAC);
		Util::Vector vectorAO = -pointA;

		if (cross(vectorABC, vectorAC)*vectorAO > 0) {
			if (vectorAC*vectorAO > 0) {
				//simplex=[A,C]
				//direction=ACxAoxAC
				list.clear();
				list.push_back(pointC);
				list.push_back(pointA);
				
				d = cross(cross(vectorAC, vectorAO), vectorAC);
				return false;
			}
			else {
				if (vectorAB*vectorAO > 0) {
					//simplex=[A,B]
					//direction=ABxAoxAB
					list.clear();
					list.push_back(pointB);
					list.push_back(pointA);
					
					d = cross(cross(vectorAB, vectorAO), vectorAB);
					return false;
				}
				else {
					//simplex=[A]
					//direction=AO
					list.clear();
					list.push_back(pointA);
					d = vectorAO;
					return false;
				}
			}
		}
		else {
			if (cross(vectorAB, vectorABC)*vectorAO > 0) {
				if (vectorAB*vectorAO > 0) {
					//simplex=[A,B]
					//direction=ABxAoxAB
					list.clear();
					list.push_back(pointB);
					list.push_back(pointA);
					
					d = cross(cross(vectorAB, vectorAO), vectorAB);
					return false;
				}
				else {
					//simplex=[A]
					//direction=AO
					list.clear();
					list.push_back(pointA);
					d = vectorAO;
					return false;
				}
			}
			else {
				//else its in triangle and we are done
				return true;
			}
		}

		
	}

	return true;

}


//RETURNS: true if collides,SIMPLEX
// false if doesnt collide, null
bool SteerLib::GJK_EPA::GJK(const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB, std::vector<Util::Vector>& simplex) {
	std::cout << "GJK--------------------" << std::endl;
	for (int i = 0; i < _shapeA.size(); i++) {
		std::cout << "shapeA:" << _shapeA[i] << std::endl;
	}
	for (int i = 0; i < _shapeB.size(); i++) {
		std::cout << "shapeB:" << _shapeB[i] << std::endl;
	}
	//get some d: (1,0,1) for example
	Util::Vector d(1, 0, 1);
	//S=Support(A,D)-Support(B,-D)
	Util::Vector point = getSupport(_shapeA, _shapeA.size(), d) - getSupport(_shapeB, _shapeB.size(), -d);
	std::cout << "support of A:" << getSupport(_shapeA, _shapeA.size(), d) << std::endl;
	std::cout << "support of B:" << getSupport(_shapeB, _shapeB.size(), -d) << std::endl;
	
	std::cout << "---------------------------"<< std::endl;
	

	//[]=S
	std::vector<Util::Vector> list;
	list.push_back(point);
	std::cout << "pointB:" << list[0] << std::endl;

	//D=-S
	Util::Vector direction = -point;//(-point.x, -point.y, -point.z);
	//std::cout << "direction:" << direction << std::endl;
	
	while (true) {
		for (int i = 0; i < list.size(); i++) {
			std::cout << "LISTING:" << list[i] << std::endl;
		}

		Util::Vector pointA = getSupport(_shapeA, _shapeA.size(), direction) - getSupport(_shapeB, _shapeB.size(), -direction);
		//if dot product between pointA and direction <0
		std::cout << "pointA:" << pointA << std::endl;
		//int n;
		//std::cin >> n;
		if (pointA*direction < 0) {
			simplex.clear();
			return false;
		}
		list.push_back(pointA);
		if (doSimplex(list, direction)) {
			simplex = list;
			return true;
		}
	}
}

int findNeerestEdgeToOrigin(std::vector<Util::Vector> polytope, float &distanceToEdge) {
	double distance = std::numeric_limits<double>::max();
	int index = 0;
	float distance_to_origin;
	std::cout << "lets do this----------------" <<std::endl;
	for (int i = 0; i < polytope.size(); i++) {

		int j;
		if (i + 1 == polytope.size()) {
			j = 0;
		}
		else {
			j = i + 1;
		}
		Util::Vector a = polytope[i];
		Util::Vector b = polytope[j];
		Util::Vector c = b-a;

		std::cout << "a: " << a << std::endl;
		std::cout << "b: " << b << std::endl;

		Util::Vector to_origin = cross((cross(c, a)), c);
		std::cout<<"to_origin="<< to_origin<<std::endl;
		to_origin=normalize(to_origin);
		std::cout << "to_origin normalized=" << to_origin << std::endl;
		distance_to_origin = to_origin*a;
		std::cout << "distance_to_origin=" << distance_to_origin << std::endl;
		if (distance_to_origin < distance) {
			distanceToEdge = distance_to_origin;
			std::cout << "distanceToEdge in method;" << distanceToEdge << std::endl;
			distance = distance_to_origin;
			index = i;
		}
	}
	return index;
}

Util::Vector SteerLib::GJK_EPA::penetration_vector(std::vector<Util::Vector> A, std::vector<Util::Vector> B, std::vector<Util::Vector> simplex) {
	std::cout << "---------EPA---------" << std::endl;
	//simplex = std::vector<Util::Vector>;
	/*simplex.clear;
	simplex.push_back( Util::Vector(4, 0, 2));
	simplex.push_back( Util::Vector(-8, 0, -2));
	simplex.push_back(Util::Vector(-1, 0, -2));
	*/
	for (int i = 0; i < simplex.size(); i++) {
		std::cout << "simplex at " <<i << ":" << simplex[i]<<std::endl;
	}

	for (int i = 0; i < A.size(); i++) {
		std::cout << "shapeA at " << i << ":" << A[i] << std::endl;
	}
	
	for (int i = 0; i < B.size(); i++) {
		std::cout << "shapeB at " << i << ":" << B[i] << std::endl;
	}

	int i = 0;

	float distanceToEdge=0;

	while (i<2) {

		
	  // obtain the feature (edge for 2D) closest to the 
	  // origin on the Minkowski Difference

	//find index of first point in nearest edge
	  int index = findNeerestEdgeToOrigin(simplex,distanceToEdge);
	  //find index of second point in nearest edge
	  int index2 = index + 1;
	  if (index2 == simplex.size()) {
		index2 = 0;
	  }
	  // obtain a new support point in the direction of the edge normal
	  /*if (i == 0) {
		  index = 1;
		  index2 = 2;
	  }
	  else if (i == 1) {
		  index = 0;
		  index2 = 1;
	  }*/

	  std::cout << "index1;" << index << std::endl;
	  std::cout << "index2;" << index2 << std::endl;
	  std::cout << "simplex at index1;" << simplex[index] << std::endl;
	  std::cout << "simplex at index2;" << simplex[index2] << std::endl;

	  //nearest edge to origin
	  Util::Vector nearestEdge = simplex[index] - simplex[index2];
	  std::cout << "nearestEdge;" << nearestEdge << std::endl;

	  //normal of nearestEdge:
	  Util::Vector nearestEdgeNormal = cross(Util::Vector(0, 1, 0), nearestEdge);
	  std::cout << "nearestEdgeNormal;" << nearestEdgeNormal << std::endl;

	  //get support point
	  Util::Vector point = getSupport(A, A.size(), nearestEdgeNormal) - getSupport(B, B.size(), -nearestEdgeNormal);
	  std::cout << "point;" << point << std::endl;
	  // check the distance from the origin to the edge against the
	  // distance p is along e.normal
	  double distancePoint = point*nearestEdgeNormal;
	  std::cout << "distance;" << distancePoint << std::endl;
	std::cout << "distanceToEdge by reference;" << distanceToEdge << std::endl;

	  if (distancePoint -distanceToEdge< 0.0001) {
		  std::cout << "we done"<< std::endl;
	    // the tolerance should be something positive close to zero (ex. 0.00001)

	    // if the difference is less than the tolerance then we can
	    // assume that we cannot expand the simplex any further and
	    // we have our solution
	    Util::Vector normal = cross(nearestEdge, Util::Vector(0, 1, 0));
	    return (normal / normal.length()) * distanceToEdge;
	  } else {
	    // we haven't reached the edge of the Minkowski Difference
	    // so continue expanding by adding the new point to the simplex
	    // in between the points that made the closest edge
	    simplex.insert(simplex.begin()+index+1, point);
	  }


	  i++;
	}
	

}



//RETURNS: true if collision
//ARGUMENTS: RETURN: return_penetration_depth: penetration depth claculated by EPA if collision
//			 RETURN: return_penetration_vector: penetration vector calculated by EPA if collision
//			 INPUT: _shapeA: shape A. array of vector points: which have x, y, z components
//			 INPUT: _shapeB: shape B. array of vector points: which have x, y, z components
//Look at the GJK_EPA.h header file for documentation and instructions
bool SteerLib::GJK_EPA::intersect(float& return_penetration_depth, Util::Vector& return_penetration_vector, const std::vector<Util::Vector>& _shapeA, const std::vector<Util::Vector>& _shapeB)
{
	//temp-------------------------

	//-------------------------------1

	std::vector<Util::Vector> simplex;
	float isColliding = GJK(_shapeA, _shapeB, simplex);
	if (isColliding == true) {
		return_penetration_vector = penetration_vector(_shapeA, _shapeB, simplex);
		return_penetration_depth = return_penetration_vector.length();
		return true;
	}
	else {
		return_penetration_depth = 0;
		return_penetration_vector.zero();
		return false;
	}
}

