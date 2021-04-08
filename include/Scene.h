#pragma once

#define _USE_MATH_DEFINES // for C++

// Includes
#include <vector>
#include <memory>
#include <random>
#include <iostream>
#include <iomanip>
#include <omp.h>
#include <chrono> // Time
#include <cmath>

#include "../include/SceneObject.h"
#include "../include/Camera.h"

class Scene {
public:
	Scene();
	~Scene();

	//void generatePhotonMap(const int NR_PHOTONS);
	static std::shared_ptr<Scene> generateScene();
	void render(std::shared_ptr<Camera> camera, const int NR_SUBSAMPLES);

private:
	std::vector<std::shared_ptr<Surface::Base>> m_sceneObjects;
	std::vector<int> m_lightIndices;

	// Add objects to scene
	void addHexagonWalls();
	void addCornellWalls();
	void addTriangle(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2, std::shared_ptr<Material> material, bool isEmissive = false);
	void addPlane(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2, const glm::vec3 v3, std::shared_ptr<Material> material, bool isEmissive = false);
	void addBox(const glm::vec3 origin, const glm::vec3 dimension, std::shared_ptr<Material> material, bool isEmissive = false);
	void addSphere(const float radius, const glm::vec3 origin, std::shared_ptr<Material> material, bool isEmissive = false);
	void addMesh(const glm::mat4 transform, const char* filePath, std::shared_ptr<Material> material, bool isEmissive = false);

	// Trace rays
	glm::vec3 traceRay(std::shared_ptr<Ray> ray, int depth = 0);
	glm::vec3 traceDiffuseRay(std::shared_ptr<Ray> ray);	// Direct light
	glm::vec3 traceShadowRay(std::shared_ptr<Ray> ray, std::shared_ptr<Ray> shadowRay);	// Local illumination, diffuse
	glm::vec3 traceRefractedRay(std::shared_ptr<Ray> ray, int depth);	// Light through transparent objects

	// Helper functions
	bool findRayIntersection(std::shared_ptr<Ray> ray);
	bool russianRoulette(const int depth);

	// Random number generator (should be in either scene or camera, where the render function is)
	std::mt19937* gen;
	std::uniform_real_distribution<float>* dis;
};