#pragma once

#define _USE_MATH_DEFINES // for C++

#include <memory>
#include <cmath>
#include <iostream>

#include "external/glm/glm/glm.hpp"
#include "external/glm/glm/gtx/vector_angle.hpp"
#include "Material.h"

class Ray;

namespace Surface {
	// Abstract base class for various scene objects
	class Base {
	public:
		virtual bool intersect(std::shared_ptr<Ray> ray) = 0;
		virtual glm::vec3 getRandomPointOnSurface(float u, float v) const = 0;

		// Getters
		float getArea() const;
		float getRadiance() const;
		std::shared_ptr<Material> getMaterial() const; // TODO: Remove, temporary for debugging

	protected: // These should be accessable for the SceneObject subclasses
		float m_surfaceArea;
		float m_emittedRadiance;
		std::shared_ptr<Material> m_material;

		explicit Base(std::shared_ptr<Material> material);
		void computeRadiance();
	};

	/**************** Sphere ****************/
	class Sphere : public Base {
	public:
		// Constructor
		Sphere(const float radius, const glm::vec3 origin, std::shared_ptr<Material> material);

		// Override: Check if given ray intersects sphere
		bool intersect(std::shared_ptr<Ray> ray) override;
		// Override: Get a random point on sphere surface
		glm::vec3 getRandomPointOnSurface(float u, float v) const override;
	private:
		float m_radius;
		glm::vec3 m_origin;

		void computeArea();
		bool solveQuadratic(const float a, const float b, const float c, float& x0, float& x1);
	};

	/**************** Triangle ****************/
	class Triangle : public Base {
	public:
		// Constructor
		Triangle(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2,	// Vertices
			std::shared_ptr<Material> material);
		/*
		Triangle(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2,	// Vertices
			const glm::vec3 n0, const glm::vec3 n1, const glm::vec3 n2,			// Normals
			std::shared_ptr<Material> material);
		*/
		// Override: Check if given ray intersects triangle
		bool intersect(std::shared_ptr<Ray> ray) override;
		// Override: Get a random point on triangle surface
		glm::vec3 getRandomPointOnSurface(float u, float v) const override;

	private:
		// Vertices
		glm::vec3 m_v0, m_v1, m_v2;

		// Normals of vertices
		glm::vec3 m_normal;

		// Edges
		glm::vec3 m_e1, m_e2;

		void computeArea();
		void computeTriangleNormal();
	};
}