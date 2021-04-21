#include "../include/SceneObject.h"

//#include "external/objloader.h"
//#include "external/vboindexer.h"

#include "../include/Ray.h"
#include "../include/OctreeAABB.h"

namespace Surface {
	/**************** Base ****************/
	Base::Base(std::shared_ptr<Material> material)
		: m_surfaceArea(0.0f), m_emittedRadiance(0.0f), m_material(material) {}

	float Base::getArea() const {
		return m_surfaceArea;
	}

	float Base::getRadiance() const {
		return m_emittedRadiance;
	}

	std::shared_ptr<Material> Base::getMaterial() const {
		return m_material;
	}

	void Base::computeRadiance() {
		if (auto emissiveMaterial = std::dynamic_pointer_cast<EmissiveMaterial>(m_material)) {
			m_emittedRadiance = emissiveMaterial->getEmissivity() / (m_surfaceArea * glm::pi<float>());
		}
	}

	/**************** Mesh ****************/
	Mesh::Mesh(glm::mat4 transform, const char* filePath, std::shared_ptr<Material> material) 
		: m_transform(transform), Base(material) {

		std::vector<glm::vec3> tmpVertices;
		std::vector<glm::vec3> tmpNormals;
		std::vector<glm::vec2> tmpUvs;

		if (!loadOBJ(filePath, tmpVertices, tmpUvs, tmpNormals)) exit(EXIT_FAILURE);

		for (int i = 0; i < (int)tmpVertices.size(); ++i) {
			tmpVertices[i] = glm::vec3(m_transform * glm::vec4(tmpVertices[i], 1));
			tmpNormals[i] = glm::vec3(m_transform * glm::vec4(tmpNormals[i], 0));
		}

		indexVBO(
			tmpVertices,
			tmpUvs,
			tmpNormals,

			m_indices,
			m_vertices,
			m_uvs,
			m_normals);
		/*
		std::cout << "Nr vertices = " << m_vertices.size() - 1 << std::endl;
		int triangleNr = 1;
		glm::vec3 v0, v1, v2;
		for (int i = 0; i < m_vertices.size(); i = i + 3) {
			v0 = m_vertices[m_indices[i + 0]];
			v1 = m_vertices[m_indices[i + 1]];
			v2 = m_vertices[m_indices[i + 2]];
			std::cout << i << ": Triangle nr " << triangleNr << std::endl;
			std::cout << "v0 = (" << v0.x << ", " << v0.y << ", " << v0.z << ")" << std::endl;
			std::cout << "v1 = (" << v1.x << ", " << v1.y << ", " << v1.z << ")" << std::endl;
			std::cout << "v2 = (" << v2.x << ", " << v2.y << ", " << v2.z << ")" << std::endl;
			triangleNr++;
		}
		*/
		std::cout << "Building octree for mesh" << std::endl;
		m_otAABB = std::make_shared<OctreeAABB>(std::make_shared<Surface::Mesh>(*this));
		std::cout << "Octree has been built" << std::endl;
	}

	bool Mesh::intersect(std::shared_ptr<Ray> ray) const {
		//std::cout << "Testing for intersection with Mesh" << std::endl;
		return m_otAABB->intersect(ray);
	}

	glm::vec3 Mesh::getRandomPointOnSurface(float u, float v) const {
		return glm::vec3(0.0f);
	}

	glm::vec3 Mesh::getMinPos() const {
		glm::vec3 v, min = m_vertices[0];
		for (int i = 0; i < (int)m_vertices.size(); ++i) {
			v = m_vertices[i];
			min.x = (v.x < min.x) ? v.x : min.x;
			min.y = (v.y < min.y) ? v.y : min.y;
			min.z = (v.z < min.z) ? v.z : min.z;
		}
		return min;
	}

	glm::vec3 Mesh::getMaxPos() const {
		glm::vec3 v, max = m_vertices[0];
		for (int i = 0; i < (int)m_vertices.size(); ++i) {
			v = m_vertices[i];
			max.x = (v.x > max.x) ? v.x : max.x;
			max.y = (v.y > max.y) ? v.y : max.y;
			max.z = (v.z > max.z) ? v.z : max.z;
		}
		return max;
	}

	glm::mat4 Mesh::getTransform() const {
		return m_transform;
	}

	int Mesh::getNrOfTriangles () const {
		return m_indices.size() / 3;
	}

	std::vector<unsigned int> Mesh::getIndices() const {
		return m_indices;
	}

	glm::vec3 Mesh::getVertex(const int i) const {
		return m_vertices[i];
	}

	glm::vec3 Mesh::getNormal(const int i) const {
		return m_normals[i];
	}

	/**************** Sphere ****************/
	Sphere::Sphere(const float radius, const glm::vec3 origin, std::shared_ptr<Material> material) 
		: m_radius(radius), m_origin(origin), Base(material) {
		computeArea();
		computeRadiance();
	}

	bool Sphere::intersect(std::shared_ptr<Ray> ray) const {
		// Math for sphere intersection (see Lecture 6 p.15)
		glm::vec3 L = ray->getStartPt() - m_origin; // Direction between ray and origin of sphere
		float a = 1.0f; // Dot product of ray direction
		float b = glm::dot((2.0f * ray->getDirection()), L);
		float c = glm::dot(L, L) - (m_radius * m_radius);

		float d0, d1;
		if (!solveQuadratic(a, b, c, d0, d1)) return false;
		if (d0 > d1) std::swap(d0, d1);

		// Check if point is inside sphere or not
		if (d0 < 0.0f) {
			d0 = d1; // Use d1 if d0 is negative
			if (d0 < 0.0f) return false; // Both d0 and d1 are negative, meaning the point does not intersect
		}

		if (glm::abs(d0) < EPSILON) return false;

		// An intersection exists
		if (ray->isIntersectionCloser(d0)) {
			glm::vec3 intersectionPt = ray->getStartPt() + d0 * ray->getDirection();
			glm::vec3 intersectionNormal = glm::normalize(intersectionPt - m_origin);

			std::shared_ptr<Intersection> newIntersection = std::make_shared<Intersection>(
				intersectionPt, intersectionNormal, d0, m_material);

			ray->setRayIntersection(newIntersection);

			return true;
		}
		return false;
	}

	glm::vec3 Sphere::getRandomPointOnSurface(float u, float v) const {
		// Uniform over hemisphere
		float inclination = glm::acos(1.0f - 2.0f * u);
		float azimuth = 2.0f * glm::pi<float>() * v;

		// Compute a random direction
		glm::vec3 randomDirection = glm::vec3(1.0f, 0.0f, 0.0f);
		randomDirection = glm::normalize(glm::rotate(
			randomDirection, 
			inclination, 
			glm::vec3(0.0f, 1.0f, 0.0f)
		));
		randomDirection = glm::normalize(glm::rotate(
			randomDirection,
			azimuth,
			glm::vec3(1.0f, 0.0f, 0.0f)
		));

		return m_origin + randomDirection * m_radius;
	}

	// TODO: Implement this function
	glm::vec3 Sphere::getNormal(const int i) const {
		return glm::vec3(0.0f);
	}

	void Sphere::computeArea() {
		m_surfaceArea = 4.0f * glm::pi<float>() * m_radius * m_radius;
	}

	bool Sphere::solveQuadratic(const float a, const float b, const float c, float& x0, float& x1) const {
		// From Scratchapixel 2.0, A Minimal Ray-Tracer: Rendering Simple Shapes: 
		// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection

		float discr = b * b - 4.0f * a * c;
		if (discr < 0.0f) return false;
		else if (glm::abs(discr) < EPSILON) x0 = x1 = -0.5f * b / a;
		else {
			float q = (b > 0.0f) ? -0.5f * (b + sqrt(discr)) : -0.5f * (b - sqrt(discr));
			x0 = q / a;
			x1 = c / q;
		}
		if (x0 > x1) std::swap(x0, x1);

		return true;
	}

	/**************** Triangle ****************/
	Triangle::Triangle(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2,	// Vertices
		std::shared_ptr<Material> material) :
		m_v0(v0), m_v1(v1), m_v2(v2), Base(material) {
		m_e1 = v1 - v0;
		m_e2 = v2 - v0;
		computeArea();
		computeTriangleNormal();
		computeRadiance();
	}

	// Möller-Trumbore intersection algorithm
	bool Triangle::intersect(std::shared_ptr<Ray> ray) const {
		// Calculate determinant
		glm::vec3 D = ray->getDirection();
		glm::vec3 P = glm::cross(D, m_e2);

		// Calculate distance from v1 to ray origin
		glm::vec3 T = ray->getStartPt() - m_v0;
		glm::vec3 Q = glm::cross(T, m_e1);

		// If determinant is near zero, ray lies in plane of triangle
		float det = glm::dot(P, m_e1);
		if (std::fabs(det) < EPSILON) return false;

		// Calculate u and v
		float invDet = 1.0f / det;
		float u = glm::dot(P, T) * invDet;
		float v = glm::dot(Q, D) * invDet;

		// Check if intersection lies outside of the plane
		if (u + v > 1.0f || u < 0.0f || v < 0.0f) return false;

		// Calculate the distance from ray to plane
		float t = glm::dot(Q, m_e2) * invDet;
	
		// Check if ray intersects the triangle and does not have a closer intersection
		if (t > EPSILON && ray->isIntersectionCloser(t)) {
			// Set up an intersection for the ray
			glm::vec3 intersectionPt = ray->getStartPt() + t * ray->getDirection();
			glm::vec3 normal = m_normal;

			std::shared_ptr<Intersection> newIntersection = std::make_shared<Intersection>(
				intersectionPt, normal, t, m_material);

			ray->setRayIntersection(newIntersection);

			return true;
		}
		return false;
	}

	glm::vec3 Triangle::getRandomPointOnSurface(float u, float v) const {
		// Random point on triangle: https://adamswaab.wordpress.com/2009/12/11/random-point-in-a-triangle-barycentric-coordinates/
		
		float rand = glm::clamp((float)std::rand() / RAND_MAX, 0.01f, 0.99f);
		float randU = rand / ((std::rand() % 8) + 2.0f);
		float randV = rand - randU;

		return (1.0f - randU - randV) * m_v0 + randU * m_v1 + randV * m_v2;
		//return (1.0f - u - v) * m_v0 + u * m_v1 + v * m_v2;
		//return (m_v0 + u * m_e1 + v * m_e2);
	}

	glm::vec3 Triangle::getNormal(const int i) const {
		return m_normal;
	}

	void Triangle::computeArea() {
		m_surfaceArea = 0.5f * glm::length(glm::cross(m_e1, m_e2));
	}

	void Triangle::computeTriangleNormal() {
		m_normal = glm::normalize(glm::cross(m_e1, m_e2));
	}
}