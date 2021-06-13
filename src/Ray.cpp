#include "../include/Ray.h"

Ray::Ray(glm::vec3 startPt, glm::vec3 direction) 
	: m_startPt(startPt), m_direction(glm::normalize(direction)), 
	m_intersection(nullptr), m_reflectionCoefficient(0.0f), m_currRefractionIndex(1.0f) {}

glm::vec3 Ray::getStartPt() const {
	return m_startPt;
}

glm::vec3 Ray::getDirection() const {
	return m_direction;
}

std::shared_ptr<Intersection> Ray::getIntersection() const {
	return m_intersection;
}

float Ray::getReflectionCoefficient() const {
	return m_reflectionCoefficient;
}

void Ray::setStartPt(const glm::vec3 startPt) {
	m_startPt = startPt;
}

void Ray::setDirection(const glm::vec3 direction) {
	m_direction = direction;
}

void Ray::setRayIntersection(std::shared_ptr<Intersection> newIntersection) {
	m_intersection = newIntersection;
}

void Ray::setRefractionIndex(const float refractionIndex) {
	m_currRefractionIndex = refractionIndex;
}

bool Ray::isInsideSurface() const {
	return (glm::dot(m_intersection->m_normal, m_direction) > 0);
}

bool Ray::isIntersectionCloser(const float distance) {
	return (!m_intersection || m_intersection->m_t > distance);
}

bool Ray::hitsEmissiveSurface() const {
	return (std::dynamic_pointer_cast<EmissiveMaterial>(m_intersection->m_material)) ? true : false;
}

bool Ray::hitsPerfectReflectorSurface() const {
	return (std::dynamic_pointer_cast<PerfectReflectorMaterial>(m_intersection->m_material)) ? true : false;
}

bool Ray::hitsTransparentSurface() const {
	return (std::dynamic_pointer_cast<TransparentMaterial>(m_intersection->m_material)) ? true : false;
}

bool Ray::hitsDiffuseSurface() const {
	// Diffuse surfaces are Lambertian and Oren-Nayar surfaces
	return (std::dynamic_pointer_cast<LambertianMaterial>(m_intersection->m_material) ||
			std::dynamic_pointer_cast<OrenNayarMaterial>(m_intersection->m_material)) ? true : false;
	//return !(std::dynamic_pointer_cast<EmissiveMaterial>(m_intersection->m_material));
	//	|| std::dynamic_pointer_cast<PerfectReflectorMaterial>(m_intersection->m_material));
}

glm::vec3 Ray::getBRDFValue(std::shared_ptr<Ray> reflectedRay) const {
	// wIn = incoming direction of ray, wOut = outgoing direction of ray
	glm::vec3 outDirection = reflectedRay->getDirection();

	/*
	// Convert to local space
	glm::mat4 worldToLocalMatrix = m_intersection->worldToLocalMatrix(m_direction);
	glm::vec3 wInLocal = glm::vec3(worldToLocalMatrix * glm::vec4(m_direction.x, m_direction.y, m_direction.z, 1.0f));
	glm::vec3 wOutLocal = glm::vec3(worldToLocalMatrix * glm::vec4(outDirection.x, outDirection.y, outDirection.z, 1.0f));

	// Normalize directions
	glm::vec3 inDirection = glm::normalize(m_direction);
	outDirection = glm::normalize(outDirection);

	// Azimuth
	float wInAzimuth = glm::atan(inDirection.y, inDirection.x);
	float wOutAzimuth = glm::atan(outDirection.y, outDirection.x);

	// Inclination
	float absIn = glm::sqrt((inDirection.x * inDirection.x + inDirection.y * inDirection.y) / inDirection.z);
	float absOut = glm::sqrt((outDirection.x * outDirection.x + outDirection.y * outDirection.y) / outDirection.z);
	float wInInclination = glm::atan(absIn);
	float wOutInclination = glm::atan(absOut);

	return m_intersection->m_material->getBRDF(wInInclination, wInAzimuth, wOutInclination, wOutAzimuth);
	*/
	return getBRDFValue(outDirection);
}

glm::vec3 Ray::getBRDFValue(const glm::vec3 direction) const {
	glm::vec3 outDirection = direction;

	// Convert to local space
	glm::mat4 worldToLocalMatrix = m_intersection->worldToLocalMatrix(m_direction);
	glm::vec3 wInLocal = glm::vec3(worldToLocalMatrix * glm::vec4(m_direction.x, m_direction.y, m_direction.z, 1.0f));
	glm::vec3 wOutLocal = glm::vec3(worldToLocalMatrix * glm::vec4(outDirection.x, outDirection.y, outDirection.z, 1.0f));

	// Normalize directions
	glm::vec3 inDirection = glm::normalize(m_direction);
	outDirection = glm::normalize(outDirection);

	// Azimuth
	float wInAzimuth = glm::atan(inDirection.y, inDirection.x);
	float wOutAzimuth = glm::atan(outDirection.y, outDirection.x);

	// Inclination
	float absIn = glm::sqrt((inDirection.x * inDirection.x + inDirection.y * inDirection.y) / inDirection.z);
	float absOut = glm::sqrt((outDirection.x * outDirection.x + outDirection.y * outDirection.y) / outDirection.z);
	float wInInclination = glm::atan(absIn);
	float wOutInclination = glm::atan(absOut);

	return m_intersection->m_material->getBRDF(wInInclination, wInAzimuth, wOutInclination, wOutAzimuth);
}

std::shared_ptr<Ray> Ray::createReflectedRay(const float rand1, const float rand2) const { // Indirect diffuse ray
	if (!m_intersection) return nullptr;

	glm::vec3 reflectedRayOrigin = m_intersection->m_intersectionPt + m_intersection->m_normal * FLT_EPSILON; // offset;
	glm::vec3 reflectedRayDirection = glm::vec3(0.0f);

	if (hitsPerfectReflectorSurface()) { // || hitsTransparentSurface()) {
		reflectedRayDirection = glm::reflect(m_direction, m_intersection->m_normal);
	}
	else { // Reflected ray gets a random direction
		glm::vec3 tangent = m_direction - glm::dot(m_direction, m_intersection->m_normal) * m_intersection->m_normal;

		float inclination = (float)glm::acos(glm::sqrt(rand1));
		float azimuth = 2.0f * glm::pi<float>() * rand2;

		reflectedRayDirection = m_intersection->m_normal;
		reflectedRayDirection = glm::normalize(glm::rotate(
			reflectedRayDirection,
			inclination,
			tangent));
		reflectedRayDirection = glm::normalize(glm::rotate(
			reflectedRayDirection,
			azimuth,
			m_intersection->m_normal));
	}

	return std::make_shared<Ray>(reflectedRayOrigin, reflectedRayDirection);
}

std::shared_ptr<Ray> Ray::createRefractedRay(std::shared_ptr<Ray> reflectedRay) {
	if (!m_intersection) return nullptr;

	// Refraction values
	float n1 = 1.0f; // Air
	float n2 = m_intersection->m_material->getRefractionIndex();

	glm::vec3 normal = m_intersection->m_normal;
	// Check if ray is inside surface
	if (isInsideSurface()) {
		normal *= -1.0f;
		std::swap(n1, n2);
	}

	glm::vec3 offset = normal * FLT_EPSILON;
	glm::vec3 refractionDirection = glm::refract(m_direction, normal, (n1 / n2));
	glm::vec3 reflectionDirection = glm::reflect(m_direction, m_intersection->m_normal);

	if (refractionDirection != glm::vec3(0.0f)) { // Refraction and reflection
		// Schlicks approximation to Fresnels equations
		calculateRadianceDistribution(n1, n2);

		glm::vec3 reflectionOrigin = m_intersection->m_intersectionPt + offset;
		glm::vec3 refractionOrigin = m_intersection->m_intersectionPt - offset;

		// Update direction and origin of reflected ray
		reflectedRay->setStartPt(reflectionOrigin);
		reflectedRay->setDirection(reflectionDirection);

		// Create the refracted ray
		return std::make_shared<Ray>(refractionOrigin, refractionDirection);
	}
	else { // Total reflection, brewster angle reached
		// Update direction and origin of reflected ray
		glm::vec3 reflectionOrigin = m_intersection->m_intersectionPt + offset;
		reflectedRay->setStartPt(reflectionOrigin);
		reflectedRay->setDirection(reflectionDirection);

		// No refracted ray is created with total reflection
		return nullptr;
	}
}

std::shared_ptr<Ray> Ray::createShadowRay(const glm::vec3 ptOnLight) const { // Direct shadow ray
	if (!m_intersection) return nullptr;
	glm::vec3 shadowRayOrigin = m_intersection->m_intersectionPt + m_intersection->m_normal * FLT_EPSILON;
	glm::vec3 shadowRayDirection = glm::normalize(ptOnLight - shadowRayOrigin);
	return std::make_shared<Ray>(shadowRayOrigin, shadowRayDirection);
}

void Ray::calculateRadianceDistribution(const float n1, const float n2) {
	// // Schlicks approximation to Fresnels equations, radiance distribution
	// See https://en.wikipedia.org/wiki/Schlick%27s_approximation for more information.
	glm::vec3 normal = (isInsideSurface()) ? -1.0f * m_intersection->m_normal : m_intersection->m_normal;
	float R0 = glm::pow((n1 - n2) / (n1 + n2), 2.0f);
	glm::vec3 halfVector = glm::normalize(glm::reflect(m_direction, normal) + -1.0f * m_direction);
	float alpha = glm::dot(normal, halfVector);
	m_reflectionCoefficient = R0 + (1.0f - R0) * glm::pow((1.0f - alpha), 5.0f);
}