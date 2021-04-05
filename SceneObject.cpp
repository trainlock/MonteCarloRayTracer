#include "SceneObject.h"
#include "Ray.h"

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
			m_emittedRadiance = emissiveMaterial->getEmissivity() / (m_surfaceArea * M_PI);
		}
	}

	/**************** Sphere ****************/
	Sphere::Sphere(const float radius, const glm::vec3 origin, std::shared_ptr<Material> material) 
		: m_radius(radius), m_origin(origin), Base(material) {
		computeArea();
		computeRadiance();
	}

	bool Sphere::intersect(std::shared_ptr<Ray> ray) {
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
		float azimuth = 2.0f * M_PI * v;

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

	void Sphere::computeArea() {
		m_surfaceArea = 4.0f * M_PI * m_radius * m_radius;
	}

	bool Sphere::solveQuadratic(const float a, const float b, const float c, float& x0, float& x1) {
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
	bool Triangle::intersect(std::shared_ptr<Ray> ray) {
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

	void Triangle::computeArea() {
		m_surfaceArea = 0.5f * glm::length(glm::cross(m_e1, m_e2));
	}

	void Triangle::computeTriangleNormal() {
		m_normal = glm::normalize(glm::cross(m_e1, m_e2));
	}
}