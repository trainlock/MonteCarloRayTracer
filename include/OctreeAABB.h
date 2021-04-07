#pragma once

#include <vector>
#include <array>
#include <memory>

#include "../external/glm/glm/glm.hpp"
#include "../external/boxOverlap.h"
//#include "../include/SceneObject.h"

//class SceneObject;
class Ray;
class OctreeNodeAABB;
class OctreeAABB;
struct AABB;
namespace Surface { class Mesh; }

// Axis Aligned Bounding Box
struct AABB {
	bool intersect(std::shared_ptr<Ray> ray) const;
	bool intersectTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v3) const;

	glm::vec3 m_min, m_max;
};

/**************** Octree Node ****************/
// A node of the octree. Each node have eight children
class OctreeNodeAABB {
public:
	OctreeNodeAABB(	 // ERROR C2664 (caused by first argument)
		std::shared_ptr<OctreeNodeAABB> parent = nullptr,	// Parent node
		int depth = 0,										// Maximum depth of tree
		std::shared_ptr<Surface::Mesh> mesh = nullptr,		// Contained object
		glm::vec3 aabbMin = glm::vec3(0.0f),				// Min boundary for axis aligned boundary box
		glm::vec3 aabbMax = glm::vec3(0.0f));				// Max boundary for axis aligned boundary box

	bool intersect(std::shared_ptr<Ray>) const;

protected:
	std::shared_ptr<Surface::Mesh> m_mesh;
	std::vector<unsigned int> m_triangleIndicies;
	AABB m_aabb;

	// Child nodes
	std::array<std::shared_ptr<OctreeNodeAABB>, 8> m_children;
	// In order:
	// m_children[0] = left bottom far
	// m_children[1] = right bottom far
	// m_children[2] = left top far
	// m_children[3] = right top far
	// m_children[4] = left bottom near
	// m_children[5] = right bottom near
	// m_children[6] = left top near
	// m_children[7] = right top near
};

/**************** Octree ****************/
// An octree containing axis aligned bounding boxes
class OctreeAABB : public OctreeNodeAABB {
public:
	OctreeAABB(std::shared_ptr<Surface::Mesh> mesh = nullptr); // ERROR C2664 (caused by first argument)
private:
};
