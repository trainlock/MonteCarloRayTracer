#include "../include/Scene.h"

Scene::Scene() {
	std::random_device rd;
	gen = new std::mt19937(rd());
	dis = new std::uniform_real_distribution<float>(0, 1);
}

Scene::~Scene() {
	delete gen;
	delete dis;
}

void Scene::addHexagonWalls() {
	// Material for scene room
	std::shared_ptr<LambertianMaterial> diffuseWhite	= std::make_shared<LambertianMaterial>(glm::vec3(1.0f));
	std::shared_ptr<LambertianMaterial> diffuseBlue		= std::make_shared<LambertianMaterial>(glm::vec3(0.0f, 1.0f, 0.0f));
	std::shared_ptr<LambertianMaterial> diffuseGreen	= std::make_shared<LambertianMaterial>(glm::vec3(0.0f, 0.0f, 1.0f));

	// Wall vertices
	glm::vec3 v0 = glm::vec3(0.0f, 6.0f, 5.0f);
	glm::vec3 v1 = glm::vec3(0.0f, 6.0f, -5.0f);
	glm::vec3 v2 = glm::vec3(10.0f, 6.0f, 5.0f);
	glm::vec3 v3 = glm::vec3(10.0f, 6.0f, -5.0f);
	glm::vec3 v4 = glm::vec3(13.0f, 0.0f, 5.0f);
	glm::vec3 v5 = glm::vec3(13.0f, 0.0f, -5.0f);
	glm::vec3 v6 = glm::vec3(10.0f, -6.0f, 5.0f);
	glm::vec3 v7 = glm::vec3(10.0f, -6.0f, -5.0f);
	glm::vec3 v8 = glm::vec3(0.0f, -6.0f, 5.0f);
	glm::vec3 v9 = glm::vec3(0.0f, -6.0f, -5.0f);
	glm::vec3 v10 = glm::vec3(-3.0f, 0.0f, 5.0f);
	glm::vec3 v11 = glm::vec3(-3.0f, 0.0f, -5.0f);
	glm::vec3 v12 = glm::vec3(5.0f, 0.0f, 5.0f);
	glm::vec3 v13 = glm::vec3(5.0f, 0.0f, -5.0f);

	// Add walls to scene
	// Floor
	addTriangle(v0, v8, v10, diffuseWhite);
	addTriangle(v0, v12, v8, diffuseWhite);
	addTriangle(v0, v12, v2, diffuseWhite);
	addTriangle(v2, v6, v12, diffuseWhite);
	addTriangle(v12, v6, v8, diffuseWhite);
	addTriangle(v2, v4, v6, diffuseWhite);
	// Roof
	addTriangle(v11, v9, v1, diffuseWhite);
	addTriangle(v1, v9, v13, diffuseWhite);
	addTriangle(v1, v13, v3, diffuseWhite);
	addTriangle(v13, v7, v3, diffuseWhite);
	addTriangle(v13, v9, v7, diffuseWhite);
	addTriangle(v3, v7, v5, diffuseWhite);
	// Wall 1
	addTriangle(v1, v2, v0, diffuseBlue);
	addTriangle(v1, v3, v2, diffuseBlue);
	// Wall 2
	addTriangle(v2, v3, v4, diffuseGreen);
	addTriangle(v4, v3, v5, diffuseGreen);
	// Wall 3
	addTriangle(v4, v5, v6, diffuseBlue);
	addTriangle(v6, v5, v7, diffuseBlue);
	// Wall 4
	addTriangle(v6, v7, v8, diffuseGreen);
	addTriangle(v8, v7, v9, diffuseGreen);
	// Wall 5
	addTriangle(v8, v9, v10, diffuseBlue);
	addTriangle(v10, v9, v11, diffuseBlue);
	// Wall 7
	addTriangle(v0, v10, v11, diffuseGreen);
	addTriangle(v0, v11, v1, diffuseGreen);
}

void Scene::addCornellWalls() {
	// Material for scene room
	std::shared_ptr<LambertianMaterial> diffuseWhite	= std::make_shared<LambertianMaterial>(glm::vec3(1.0f));
	std::shared_ptr<LambertianMaterial> diffuseBlue		= std::make_shared<LambertianMaterial>(glm::vec3(0.0f, 1.0f, 0.0f));
	std::shared_ptr<LambertianMaterial> diffuseGreen	= std::make_shared<LambertianMaterial>(glm::vec3(0.0f, 0.0f, 1.0f));
	std::shared_ptr<LambertianMaterial> diffuseRed		= std::make_shared<LambertianMaterial>(glm::vec3(1.0f, 0.0f, 0.0f));

	glm::vec3 v0 = glm::vec3(-1.5f, -1.0f, -1.0f);
	glm::vec3 v1 = glm::vec3(1.5f, -1.0f, -1.0f);
	glm::vec3 v2 = glm::vec3(1.5f, 1.0f, -1.0f);
	glm::vec3 v3 = glm::vec3(-1.5f, 1.0f, -1.0f);
	glm::vec3 v4 = glm::vec3(-1.5f, -1.0f, 4.0f);
	glm::vec3 v5 = glm::vec3(1.5f, -1.0f, 4.0f);
	glm::vec3 v6 = glm::vec3(1.5f, 1.0f, 4.0f);
	glm::vec3 v7 = glm::vec3(-1.5f, 1.0f, 4.0f);

	addPlane(v0, v1, v2, v3, diffuseRed);	// Back wall
	addPlane(v4, v7, v6, v5, diffuseWhite);	// Front wall
	addPlane(v0, v3, v7, v4, diffuseBlue);	// Left wall
	addPlane(v1, v5, v6, v2, diffuseGreen);	// Right wall
	addPlane(v2, v6, v7, v3, diffuseWhite);	// Roof
	addPlane(v0, v4, v5, v1, diffuseWhite);	// Floor
}

void Scene::addTriangle(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2, std::shared_ptr<Material> material, bool isEmissive) {
	std::shared_ptr<Surface::Triangle> triangle = std::make_shared<Surface::Triangle>(v0, v1, v2, material);
	m_sceneObjects.push_back(triangle);
	if (isEmissive) {
		m_lightIndices.push_back(m_sceneObjects.size() - 1);
	}
}

void Scene::addPlane(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2, const glm::vec3 v3, std::shared_ptr<Material> material, bool isEmissive) {
	// Create two triangles
	std::shared_ptr<Surface::Triangle> t1 = std::make_shared<Surface::Triangle>(v0, v1, v2, material);
	std::shared_ptr<Surface::Triangle> t2 = std::make_shared<Surface::Triangle>(v2, v3, v0, material);

	// Add to list of scene objects
	m_sceneObjects.push_back(t1);
	m_sceneObjects.push_back(t2);

	if (isEmissive) {
		m_lightIndices.push_back(m_sceneObjects.size() - 1);
		m_lightIndices.push_back(m_sceneObjects.size() - 2);
	}
}

void Scene::addBox(const glm::vec3 origin, const glm::vec3 dimension, std::shared_ptr<Material> material, bool isEmissive) {
	float w = dimension.x / 2.0f;
	float h = dimension.y / 2.0f;
	float d = dimension.z / 2.0f;

	// Box vertices
	glm::vec3 v0 = glm::vec3(-w, -h, -d) + origin;
	glm::vec3 v1 = glm::vec3(w, -h, -d) + origin;
	glm::vec3 v2 = glm::vec3(w, h, -d) + origin;
	glm::vec3 v3 = glm::vec3(-w, h, -d) + origin;
	glm::vec3 v4 = glm::vec3(-w, -h, d) + origin;
	glm::vec3 v5 = glm::vec3(w, -h, d) + origin;
	glm::vec3 v6 = glm::vec3(w, h, d) + origin;
	glm::vec3 v7 = glm::vec3(-w, h, d) + origin;

	// Add box sides
	addPlane(v3, v2, v1, v0, material, isEmissive);	// Back wall
	addPlane(v4, v5, v6, v7, material, isEmissive);	// Front wall
	addPlane(v0, v4, v7, v3, material, isEmissive);	// Left wall
	addPlane(v1, v2, v6, v5, material, isEmissive);	// Right wall
	addPlane(v2, v3, v7, v6, material, isEmissive);	// Roof
	addPlane(v0, v1, v5, v4, material, isEmissive);	// Floor
}

// TODO: Change push_back to emplace_back
void Scene::addSphere(const float radius, const glm::vec3 origin, std::shared_ptr<Material> material, bool isEmissive) {
	std::shared_ptr<Surface::Sphere> sphere = std::make_shared<Surface::Sphere>(radius, origin, material);
	m_sceneObjects.push_back(sphere);
	if (isEmissive) {
		m_lightIndices.push_back(m_sceneObjects.size() - 1);
	}
}

std::shared_ptr<Scene> Scene::generateScene() {
	std::shared_ptr<Scene> scene = std::make_shared<Scene>();

	// Add scene walls
	//scene->addHexagonWalls();
	scene->addCornellWalls();

	std::shared_ptr<LambertianMaterial> diffuseCyan = std::make_shared<LambertianMaterial>(glm::vec3(0.0f, 1.0f, 1.0f));
	//scene->addBox(glm::vec3(-0.4f, -0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), diffuseCyan);
	scene->addBox(glm::vec3(-0.4f, -0.5f, 0.7f), glm::vec3(0.5f, 0.5f, 0.5f), diffuseCyan);

	std::shared_ptr<TransparentMaterial> glas = std::make_shared<TransparentMaterial>(1.5f);
	std::shared_ptr<PerfectReflectorMaterial> mirror = std::make_shared<PerfectReflectorMaterial>();
	//std::shared_ptr<LambertianMaterial> diffuseCyan = std::make_shared<LambertianMaterial>(glm::vec3(0.0f, 1.0f, 1.0f));
	//std::shared_ptr<OrenNayarMaterial> onCyan = std::make_shared<OrenNayarMaterial>(glm::vec3(0.0f, 1.0f, 1.0f), 5.0f);
	//scene->addSphere(0.3f, glm::vec3(0.4f, -0.5f, 0.0f), glas);
	scene->addSphere(0.3f, glm::vec3(0.4f, -0.5f, 0.7f), glas);

	// Add light source
	std::shared_ptr<EmissiveMaterial> emissiveWhite = std::make_shared<EmissiveMaterial>(glm::vec3(1.0f), 30.0f);
	//glm::vec3 lv0 = glm::vec3(5.0f, -2.0f, -4.99f);
	//glm::vec3 lv1 = glm::vec3(7.0f, 0.0f, -4.99f);
	//glm::vec3 lv2 = glm::vec3(5.0f, 2.0f, -4.99f);

	//glm::vec3 lv0 = glm::vec3(-0.35f, 0.99f, 0.35f);
	//glm::vec3 lv1 = glm::vec3(-0.35f, 0.99f, -0.35f);
	//glm::vec3 lv2 = glm::vec3(0.35f, 0.99f, -0.35f);
	//glm::vec3 lv3 = glm::vec3(0.35f, 0.99f, 0.35f);

	glm::vec3 lv0 = glm::vec3(-0.3f, 0.99f, 1.2f);
	glm::vec3 lv1 = glm::vec3(-0.3f, 0.99f, 0.6f);
	glm::vec3 lv2 = glm::vec3(0.3f, 0.99f, 0.6f);
	//glm::vec3 lv3 = glm::vec3(0.3f, 0.99f, 0.55f);
	scene->addTriangle(lv0, lv1, lv2, emissiveWhite, true);

	return scene;
}

void Scene::render(std::shared_ptr<Camera> camera, const int NR_SUBSAMPLES) {
	time_t currTime, startRenderTime;

	float renderedPercent = 0.0f;
	std::cout << "Rendering started" << std::endl;
	std::cout << renderedPercent << "% finished" << std::endl;

	time(&startRenderTime);

	// Variables
	int width = camera->getPixelWidth();
	int height = camera->getPixelHeight();

	// Randomizer
	std::random_device rd;
	std::mt19937 genHalf(rd());
	std::uniform_real_distribution<float> disHalf(-0.5, 0.5f);

	// Loop over all pixels
	for (int x = 0; x < width; ++x) {
		#pragma omp parallel for
		for (int y = 0; y < height; ++y) {
			glm::vec3 pixelColour = glm::vec3(0.0f);
			glm::vec3 totalColour = glm::vec3(0.0f);
			for (int subsample = 0; subsample < NR_SUBSAMPLES; ++subsample) {
				std::shared_ptr<Ray> ray = camera->castCameraRay(
					x,						// Pixel x
					(height - y - 1),		// Pixel y
					disHalf(genHalf),		// Parameter x (>= -0.5, < 0.5), for subsampling
					disHalf(genHalf));		// Parameter y (>= -0.5, < 0.5), for subsampling
				pixelColour += traceRay(ray);
			}
			totalColour += pixelColour / (float)NR_SUBSAMPLES; 
			camera->setPixelValues(x, y, totalColour);
		}
		renderedPercent = (x + 1) * 100 / float(width);
		time(&currTime);
		double renderTimeElapsed = difftime(currTime, startRenderTime);
		double renderTimeLeft = (renderTimeElapsed / renderedPercent) * (100 - renderedPercent);

		int hours = (int)renderTimeLeft / (60 * 60);
		int minutes = (int(renderTimeLeft) % (60 * 60) / 60);
		int seconds = int(renderTimeLeft) % 60;

		std::cout << renderedPercent << "% of rendering finished" << std::setw(30);
		std::cout << "Estimated time left: " << hours << "h:" << minutes << "m:" << seconds << "s" << std::endl;
	}
}

// Path tracer that returns the colour of the hit surface
glm::vec3 Scene::traceRay(std::shared_ptr<Ray> ray, int depth) {
	// Check if ray intersects an objects surface
	if (!findRayIntersection(ray)) return glm::vec3(0.0f);

	// Russian roulette
	bool terminateRay = russianRoulette(depth);
	//if (terminateRay) return glm::vec3(0.0f);

	// Variables
	glm::vec3 indirectLight = glm::vec3(0.0f);	// Diffuse light (local and global)
	glm::vec3 directLight = glm::vec3(0.0f);	// Specular light
	glm::vec3 caustics = glm::vec3(0.0f);
	glm::vec3 brdf = glm::vec3(0.0f);

	// Create reflected ray
	std::shared_ptr<Ray> reflectedRay = ray->createReflectedRay((*dis)(*gen), (*dis)(*gen));
	brdf = ray->getBRDFValue(reflectedRay);
	// Check if a light source is hit
	if (ray->hitsEmissiveSurface()) indirectLight = brdf;
	else if (ray->hitsTransparentSurface() && !terminateRay) indirectLight += traceRefractedRay(ray, depth);
	else if (!ray->hitsDiffuseSurface() || !terminateRay) indirectLight += traceRay(reflectedRay, depth + 1) * brdf;

	// Compute direct lightning
	if (ray->hitsDiffuseSurface()) {
		directLight = traceDiffuseRay(ray); // Direct lightning
		// TODO: Compute caustics
	}

	return glm::clamp(directLight + indirectLight, 0.0f, 1.0f);
}

glm::vec3 Scene::traceRefractedRay(std::shared_ptr<Ray> ray, int depth) {
	std::shared_ptr<Ray> reflectedRay = std::make_shared<Ray>(glm::vec3(0.0f), glm::vec3(0.0f));
	std::shared_ptr<Ray> refractedRay = ray->createRefractedRay(reflectedRay);
	if (!refractedRay) { // Total reflection
		return traceRay(reflectedRay, depth + 1);
	}
	else {
		float R = ray->getReflectionCoefficient();
		glm::vec3 reflectedLight = traceRay(reflectedRay, depth + 1) * R;
		glm::vec3 refractedLight = traceRay(refractedRay, depth + 1) * (1.0f - R);
		return reflectedLight + refractedLight;
	}
}

glm::vec3 Scene::traceDiffuseRay(std::shared_ptr<Ray> ray) {
	glm::vec3 totalLightContribution = glm::vec3(0.0f);
	glm::vec3 lightContribution, ptOnEmissive;
	const int nrShadowRays = 1;

	for (int lightIndex : m_lightIndices) {
		lightContribution = glm::vec3(0.0f);
		std::shared_ptr<Surface::Base> emissive = m_sceneObjects[lightIndex];
		for (int i = 0; i < nrShadowRays; i++) {
			// Create a shadow ray from the ray intersection point towards the a random point on the light
			ptOnEmissive = emissive->getRandomPointOnSurface((*dis)(*gen), (*dis)(*gen));
			std::shared_ptr<Ray> shadowRay = ray->createShadowRay(ptOnEmissive);

			lightContribution += traceShadowRay(ray, shadowRay);
		}
		lightContribution *= (emissive->getRadiance() * emissive->getArea()) / nrShadowRays / (glm::pi<float>() * 2.0f);
		totalLightContribution += lightContribution;
	}

	return glm::clamp(totalLightContribution, 0.0f, 1.0f);
}

glm::vec3 Scene::traceShadowRay(std::shared_ptr<Ray> ray, std::shared_ptr<Ray> shadowRay) {
	glm::vec3 shadowRayDirection = glm::normalize(shadowRay->getDirection());

	// Find an intersection for the shadow ray
	// http://www.pbr-book.org/3ed-2018/Light_Transport_I_Surface_Reflection/Path_Tracing.html
	if (!findRayIntersection(shadowRay) || !shadowRay->hitsEmissiveSurface()) return glm::vec3(0.0f);

	// Compute the geometric term
	// Incoming angle
	float cosBeta = glm::dot(shadowRayDirection, ray->getIntersection()->m_normal);
	if (cosBeta < 0.0f) return glm::vec3(0.0f);

	// Outgoing angle
	float cosAlpha = glm::dot(-1.0f * shadowRayDirection, shadowRay->getIntersection()->m_normal);
	if (cosAlpha < 0.0f) return glm::vec3(0.0f);

	float lengthShadowRay = glm::length(shadowRayDirection);
	float lengthSquared = lengthShadowRay * lengthShadowRay;

	// Get brdf of surface
	glm::vec3 brdf = ray->getBRDFValue(shadowRay);

	// Return shadow ray contribution
	return (brdf * (cosAlpha * cosBeta) / lengthSquared);
}

bool Scene::findRayIntersection(std::shared_ptr<Ray> ray) {
	bool hasIntersected = false;
	for (auto& object : m_sceneObjects) {
		if (object->intersect(ray) && !hasIntersected) {
			hasIntersected = true;
		}
	}
	return hasIntersected;
}

bool Scene::russianRoulette(const int depth) {
	// Russian roulette
	// http://www.pbr-book.org/3ed-2018/Monte_Carlo_Integration/Russian_Roulette_and_Splitting.html
	float rand = (*dis)(*gen);
	float nonTerminationProbability = (depth == 0) ? 1.0f : 0.8f;

	return (rand > nonTerminationProbability || depth > 3);
}