#include "../include/OctreeAABB.h"

//#include "../external/boxOverlap.h"

#include "../include/SceneObject.h"
#include "../include/Ray.h"

/**************** AABB ****************/
bool AABB::intersect(std::shared_ptr<Ray> ray) const {
	glm::vec3 rayOrigin = ray->getStartPt();
	glm::vec3 rayDirection = ray->getDirection();

	glm::vec3 dirfrac(1.0f / rayDirection.x, 1.0f / rayDirection.y, 1.0f / rayDirection.z);

	// Setup bounding box corner intersection distance
	float tx0 = (m_min.x - rayOrigin.x) * dirfrac.x;
	float tx1 = (m_max.x - rayOrigin.x) * dirfrac.x;
	float ty0 = (m_min.y - rayOrigin.y) * dirfrac.y;
	float ty1 = (m_max.y - rayOrigin.y) * dirfrac.y;
	float tz0 = (m_min.z - rayOrigin.z) * dirfrac.z;
	float tz1 = (m_max.z - rayOrigin.z) * dirfrac.z;

	// Max and min intersection distance t
	float tMin = glm::max(glm::max(glm::min(tx0, tx1), glm::min(ty0, ty1)), glm::min(tz0, tz1));
	float tMax = glm::min(glm::min(glm::max(tx0, tx1), glm::max(ty0, ty1)), glm::max(tz0, tz1));

	// If tMin < 0 the ray intersects the AABB but is behind it
	if (tMin < 0) return false;

	// If tMin > tMax then ray doesn't intersect the AABB
	if (tMin > tMax) return false;
	//std::cout << "AABB::intersect: Ray intersected bounding box" << std::endl;
	return true;
}

bool AABB::intersectTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) const {
	glm::vec3 centerPt = (m_min + m_max) / 2.0f;
	glm::vec3 scale = (m_max - centerPt) / 1.0f;

	// Convert to format used by triBoxOverlap (int boxOverlap.h)
	float boxCenter[3] = { centerPt[0], centerPt[1], centerPt[2] };
	float boxHalfSize[3] = { scale[0], scale[1], scale[2] };
	float triVerts[3][3] = {
		{v0.x, v0.y, v0.z},
		{v1.x, v1.y, v1.z},
		{v2.x, v2.y, v2.z} };

	bool hasIntersected = (triBoxOverlap(boxCenter, boxHalfSize, triVerts) == 1);
	//std::cout << "Is triangle intersected? " << (hasIntersected ? "Yes" : "No") << std::endl;
	return hasIntersected;
}

/**************** Octree Node ****************/
OctreeNodeAABB::OctreeNodeAABB(
	std::shared_ptr<OctreeNodeAABB> parent,		// Parent node
	int depth,									// Maximum depth of tree
	std::shared_ptr<Surface::Mesh> mesh,		// Contained object
	glm::vec3 aabbMin,							// Min boundary for axis aligned boundary box
	glm::vec3 aabbMax) : m_mesh(mesh) {			// Max boundary for axis aligned boundary box
	//m_mesh = mesh;
	m_aabb.m_min = aabbMin;
	m_aabb.m_max = aabbMax;

	// Find which triangles that are contained in this AABB
	// If the node has no parent then this is the root
	std::vector<unsigned int> indices = (!parent) ? mesh->getIndices() : parent->m_triangleIndicies;

	//std::cout << "Triangle indices.size() = " << (int)indices.size() << std::endl;
	//int triangleNr = 1;
	//glm::vec3 v0, v1, v2;
	// Check which triangles are contained in the node
	for (int i = 0; i < (int)indices.size(); i = i + 3) {
		/*
		v0 = mesh->getVertex(indices[i + 0]);
		v1 = mesh->getVertex(indices[i + 1]);
		v2 = mesh->getVertex(indices[i + 2]);
		std::cout << i << ": Triangle nr " << triangleNr << std::endl;
		std::cout << "v0 = (" << v0.x << ", " << v0.y << ", " << v0.z << ")" << std::endl;
		std::cout << "v1 = (" << v1.x << ", " << v1.y << ", " << v1.z << ")" << std::endl;
		std::cout << "v2 = (" << v2.x << ", " << v2.y << ", " << v2.z << ")" << std::endl;
		*/
		if (m_aabb.intersectTriangle(
			mesh->getVertex(indices[i + 0]),
			mesh->getVertex(indices[i + 1]),
			mesh->getVertex(indices[i + 2]))) {
			// Insert triangle in node
			m_triangleIndicies.push_back(indices[i + 0]);
			m_triangleIndicies.push_back(indices[i + 1]);
			m_triangleIndicies.push_back(indices[i + 2]);
		}
		//triangleNr++;
	}

	//std::cout << "Depth = " << depth << ", triangleIndices.size() = " << (int)m_triangleIndicies.size() << std::endl;
	if (depth == 0 || m_triangleIndicies.size() <= 3 * 16) {
		// Base case
		for (int i = 0; i < 8; ++i) {
			m_children[i] = nullptr;
		}
		//std::cout << "OctreeNodeAABB::constructor: Base case" << std::endl;
	}
	else {
		// Recursion to create all 8 child nodes
		glm::vec3 childAABBMin, childAABBMax;
		for (int i = 0; i < 8; ++i) {
			// Bounding box min
			childAABBMin = glm::vec3(
				(i % 2 == 0)		? m_aabb.m_min.x : (m_aabb.m_min.x + m_aabb.m_max.x) / 2,
				((i / 2) % 2 == 0)	? m_aabb.m_min.y : (m_aabb.m_min.y + m_aabb.m_max.y) / 2,
				((i / 4) % 2 == 0)	? m_aabb.m_min.z : (m_aabb.m_min.z + m_aabb.m_max.z) / 2);
			// Bounding box max
			childAABBMax = glm::vec3(
				(i % 2 == 0)		? (m_aabb.m_min.x + m_aabb.m_max.x) / 2 : m_aabb.m_max.x,
				((i / 2) % 2 == 0)	? (m_aabb.m_min.y + m_aabb.m_max.y) / 2 : m_aabb.m_max.y,
				((i / 4) % 2 == 0)	? (m_aabb.m_min.z + m_aabb.m_max.z) / 2 : m_aabb.m_max.z);
			// Create child node
			m_children[i] = std::make_shared<OctreeNodeAABB>(
				std::make_shared<OctreeNodeAABB>(*this), depth - 1, mesh,
				childAABBMin, childAABBMax);
		}
		//std::cout << "OctreeNodeAABB::constructor: Created child nodes to the octree node" << std::endl;
	}
}

bool OctreeNodeAABB::intersect(std::shared_ptr<Ray> ray) const {
	// Check if node contains any triangles
	if (m_triangleIndicies.size() == 0) return false;
	else if (m_children[0] == nullptr) {
		// Reached a leaf node in the octree
		//std::cout << std::endl << "OctreeNodeAABB::intersect: At leaf node" << std::endl;
		bool hasIntersected = false;
		glm::vec3 n0, n1, n2;	// Normals
		glm::vec3 v0, v1, v2;	// Vertices
		glm::vec3 e1, e2;		// Edges
		glm::vec3 P, T, Q;
		glm::vec3 D = ray->getDirection();
		glm::vec3 rayOrigin = ray->getStartPt();
		float det, invDet, u, v, t;
		float tMin = 10000.0f;

		//int triangleNr = 1;
		// Check intersection for all triangles in this node
		for (int i = 0; i < (int)m_triangleIndicies.size(); i = i + 3) {
			//std::cout << "Vertices " << i << "-" << i + 3 << ", triangle nr " << triangleNr << std::endl;
			//triangleNr++;
 			// Möller–Trumbore intersection algorithm for triangle
			v0 = m_mesh->getVertex(m_triangleIndicies[i + 0]);
			v1 = m_mesh->getVertex(m_triangleIndicies[i + 1]);
			v2 = m_mesh->getVertex(m_triangleIndicies[i + 2]);

			// Find edges for triangle
			e1 = v1 - v0;
			e2 = v2 - v0;

			// Calculate determinant
			P = glm::cross(D, e2);

			// Calculate distance from v1 to ray origin
			T = rayOrigin - v0;
			Q = glm::cross(T, e1);

			// If determinant is near zero, then the ray lies in plane of triangle
			det = glm::dot(e1, P);
			if (std::fabs(det) < EPSILON) {
				hasIntersected = false; 
				//std::cout << "Determinat is near zero; Continue" << std::endl;
				continue;
			}

			// Calculate u and v
			invDet = 1.0f / det;
			u = glm::dot(P, T) * invDet;
			v = glm::dot(Q, D) * invDet;

			// Check if intersection lies outside of the plane
			if (u + v > 1.0f || u < 0.0f || v < 0.0f) {
				hasIntersected = false;
				//std::cout << "Intersections is outside of plane; Continue" << std::endl;
				continue;
			}

			// Calculate the distance from ray to plane
			t = glm::dot(Q, e2) * invDet;

			if (t > EPSILON && t < tMin) {
				//std::cout << "OctreeNodeAABB::intersect: Ray intersected node" << std::endl;
				//std::cout << "t = " << t << std::endl;
				tMin = t;

				// Calculate the normal of the triangle
				n0 = m_mesh->getNormal(m_triangleIndicies[i + 0]);
				n1 = m_mesh->getNormal(m_triangleIndicies[i + 1]);
				n2 = m_mesh->getNormal(m_triangleIndicies[i + 2]);

				// Interpolate to find the normal
				glm::vec3 normal = (1.0f - u - v) * n0 + u * n1 + v * n2;

				// Set up an intersection for the ray
				glm::vec3 intersectionPt = ray->getStartPt() + t * ray->getDirection();

				std::shared_ptr<Intersection> newIntersection = std::make_shared<Intersection>(
					intersectionPt, normal, t, m_mesh->getMaterial());

				ray->setRayIntersection(newIntersection);
				hasIntersected = true;
			}
			hasIntersected = false;
		}
		return hasIntersected;
	}
	else {
		// Check intersection with child nodes
		//std::cout << "OctreeNodeAABB::intersect: Not a leaf node, checking child nodes" << std::endl;
		bool hasIntersected = false;
		for (int i = 0; i < 8; ++i) {
			// Check if ray intersects the bounding box of child node
			if (m_children[i]->m_aabb.intersect(ray)) {
				// Check if ray intersects an object in child node
				if (m_children[i]->intersect(ray)) {
					hasIntersected = true;
				}
			}
		}
		return hasIntersected;
	}
	return false;
}

/**************** Octtree ****************/
OctreeAABB::OctreeAABB(std::shared_ptr<Surface::Mesh> mesh)
	: OctreeNodeAABB(nullptr, 8, mesh, mesh->getMinPos(), mesh->getMaxPos()) {}