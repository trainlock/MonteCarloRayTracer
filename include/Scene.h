#pragma once

#ifndef SCENE_H
#define SCENE_H

// Includes
#include <vector>
#include <memory>
#include <random>
#include <iostream>
#include <iomanip>
#include <omp.h>
#include <chrono> // Time

#include "../external/kdtree++/kdtree.hpp"

#include "../include/SceneObject.h"
#include "../include/Photon.h"
#include "../include/Camera.h"

class Scene {
public:
	Scene();
	~Scene();

	enum renderMode {
		CAUSTICS, MONTE_CARLO,
	};

	void setNrSubsamples(const int nrSubsamples);
	void setNrPhotonEmission(const int nrPhotonEmission);
	int getNrSubsamples() const;
	int getNrPhotonEmission() const;

	static std::shared_ptr<Scene> generateScene();
	void generatePhotonMap(const int NR_PHOTONS);
	void render(std::shared_ptr<Camera> camera);

private:
	const static int MAX_DEPTH = 2;
	int m_nrSubsamples, m_nrPhotonEmission;
	std::vector<int> m_lightIndices;
	std::vector<std::shared_ptr<Surface::Base>> m_sceneObjects;
	KDTree::KDTree<3, KDTreeNode> m_photonMap;

	// Add objects to scene
	void addHexagonWalls();
	void addCornellWalls();
	void addTriangle(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2, std::shared_ptr<Material> material, bool isEmissive = false);
	void addPlane(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2, const glm::vec3 v3, std::shared_ptr<Material> material, bool isEmissive = false);
	void addBox(const glm::vec3 origin, const glm::vec3 dimension, std::shared_ptr<Material> material, bool isEmissive = false);
	void addSphere(const float radius, const glm::vec3 origin, std::shared_ptr<Material> material, bool isEmissive = false);
	void addMesh(const glm::mat4 transform, const char* filePath, std::shared_ptr<Material> material, bool isEmissive = false);

	// Construction of photon map
	std::shared_ptr<Ray> castLightRay(const int pickedLight = 0);
	glm::vec3 tracePhotonRay(std::shared_ptr<Ray> ray, glm::vec3 photonRadiance = glm::vec3(0.0f), int depth = 0);
	glm::vec3 traceRefractedPhotonRay(std::shared_ptr<Ray> ray, glm::vec3 photonRadiance = glm::vec3(0.0f), int depth = 0);
	void addPhotonToMap(std::shared_ptr<Ray> ray, glm::vec3 photonRadiance, int depth);

	// Trace rays
	glm::vec3 traceRay(std::shared_ptr<Ray> ray, const int renderMode, int depth = 0);
	glm::vec3 traceRefractedRay(std::shared_ptr<Ray> ray, int depth); // Light through transparent objects
	glm::vec3 traceDiffuseRay(std::shared_ptr<Ray> ray); // Direct light
	glm::vec3 traceShadowRay(std::shared_ptr<Ray> ray, std::shared_ptr<Ray> shadowRay);	// Local illumination, diffuse
	glm::vec3 traceCausticsRay(std::shared_ptr<Ray> ray);

	// Helper functions
	bool findRayIntersection(std::shared_ptr<Ray> ray);
	bool russianRoulette(const int depth);

	// Random number generator (should be in either scene or camera, where the render function is)
	std::mt19937* gen;
	std::uniform_real_distribution<float>* dis;
};

#endif // SCENE_H