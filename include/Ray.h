#pragma once

#ifndef RAY_H
#define RAY_H 

#include <memory>
#include <iostream>

#include "../external/glm/glm/glm.hpp"
#include "../external/glm/glm/gtx/vector_angle.hpp"

#include "../include/Material.h"

/**************** Intersection ****************/
struct Intersection {
	Intersection(const glm::vec3 pt, const glm::vec3 normal, const float t, std::shared_ptr<Material> material) 
		: m_intersectionPt(pt), m_normal(normal), m_t(t), m_material(material) {}

	// World to local matrix
	glm::mat4 worldToLocalMatrix(glm::vec3 direction) {
		glm::vec3 axisZ = m_normal;
		glm::vec3 axisX = direction - glm::dot(direction, axisZ) * axisZ;
		glm::vec3 axisY = glm::cross(-1.0f * axisX, axisZ);

		return glm::inverse(glm::mat4(
			axisX.x, axisX.y, axisX.z, 0.0f,
			axisY.x, axisY.y, axisY.z, 0.0f,
			axisZ.x, axisZ.y, axisZ.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));
	}

	// Variables
	float m_t = 10000.0f; // distance to ray origin
	glm::vec3 m_intersectionPt, m_normal;
	std::shared_ptr<Material> m_material;
};

/**************** Ray ****************/
/**
* COMMENT DESCRIBING A CLASS
**/
class Ray {
public:
	// Constructor
	Ray(glm::vec3 startPt, glm::vec3 direction);

	// Getters and setters
	glm::vec3 getStartPt() const;
	glm::vec3 getDirection() const;

	/**
	  *   \brief A foo method.
	  *
	  *   More complete description of foo.
	  *
	  *   \param i A foo parameter.
	  *   \return An int
	  *
	  **/
	std::shared_ptr<Intersection> getIntersection() const;
	float getReflectionCoefficient() const;
	void setStartPt(const glm::vec3 startPt);
	void setDirection(const glm::vec3 direction);
	void setRayIntersection(std::shared_ptr<Intersection> newIntersection);
	void setRefractionIndex(const float refractionIndex);

	bool isInsideSurface() const;
	bool isIntersectionCloser(const float distance = 0.0f);
	bool hitsEmissiveSurface() const;
	bool hitsPerfectReflectorSurface() const;
	bool hitsTransparentSurface() const;
	bool hitsDiffuseSurface() const;

	glm::vec3 getBRDFValue(std::shared_ptr<Ray> reflectedRay) const;
	glm::vec3 getBRDFValue(const glm::vec3 direction) const;

	std::shared_ptr<Ray> createReflectedRay(const float rand1, const float rand2) const;
	std::shared_ptr<Ray> createRefractedRay(std::shared_ptr<Ray> reflectedRay);
	std::shared_ptr<Ray> createShadowRay(const glm::vec3 ptOnLight) const;

private:
	glm::vec3 m_startPt, m_direction;
	std::shared_ptr<Intersection> m_intersection;
	float m_reflectionCoefficient, m_currRefractionIndex;

	void calculateRadianceDistribution(const float n1, const float n2);
};

#endif // RAY_H
