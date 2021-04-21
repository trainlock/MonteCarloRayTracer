#include "../include/Material.h"

/**************** Base ****************/
Material::Material() 
	: m_rho(glm::vec3(0.0f)), m_rhoOverPi(glm::vec3(0.0f)), m_refractionIndex(1.0f) {}

Material::Material(float refractionIndex)
	: m_rho(glm::vec3(0.0f)), m_rhoOverPi(glm::vec3(0.0f)), m_refractionIndex(refractionIndex) {}

Material::Material(glm::vec3 reflectionCoefficient) 
	: m_rho(reflectionCoefficient), m_refractionIndex(1.0f) {
	m_rhoOverPi = glm::one_over_pi<float>() * m_rho;
}

float Material::getRefractionIndex() const {
	return m_refractionIndex;
}

glm::vec3 Material::getColour() const {
	return m_rho;
}

/**************** Lambertian ****************/
LambertianMaterial::LambertianMaterial(const glm::vec3 reflectionCoefficient)
	: Material(reflectionCoefficient) {}

glm::vec3 LambertianMaterial::getBRDF(const float wInInclination, const float wInAzimuth,
	const float wOutInclination, const float wOutAzimuth) const {
	return m_rho;
}

int LambertianMaterial::getTypeOfMaterial() const {
	return 1;
}

/**************** Oren-Nayar ****************/
OrenNayarMaterial::OrenNayarMaterial(const glm::vec3 reflectionCoefficient, const float roughness)
	: Material(reflectionCoefficient), m_roughness(roughness) {}

glm::vec3 OrenNayarMaterial::getBRDF(const float wInInclination, const float wInAzimuth,
	const float wOutInclination, const float wOutAzimuth) const {
	float sigma = m_roughness * m_roughness;

	float A = 1.0f - 0.5f * sigma / (sigma + 0.57f); // 0.33f
	float B = 0.45f * sigma / (sigma + 0.09f);

	float alpha = glm::max(wInAzimuth, wOutAzimuth);
	float beta = glm::min(wInAzimuth, wOutAzimuth);

	return m_rhoOverPi * (A + B * glm::max(0.0f, glm::cos(wInInclination - wOutInclination)) * glm::sin(alpha) * glm::sin(beta));
}

int OrenNayarMaterial::getTypeOfMaterial() const {
	return 2;
}

/**************** Perfect Reflector ****************/
PerfectReflectorMaterial::PerfectReflectorMaterial()
	: Material() {}

glm::vec3 PerfectReflectorMaterial::getBRDF(const float wInInclination, const float wInAzimuth,
	const float wOutInclination, const float wOutAzimuth) const {
	return glm::vec3(1.0f); // No loss
}

int PerfectReflectorMaterial::getTypeOfMaterial() const {
	return 3;
}

/**************** Transparent ****************/
TransparentMaterial::TransparentMaterial(const float refractionIndex)
	: Material(refractionIndex) {}

glm::vec3 TransparentMaterial::getBRDF(const float wInInclination, const float wInAzimuth,
	const float wOutInclination, const float wOutAzimuth) const {
	return glm::vec3(1.0f); // No loss
}

int TransparentMaterial::getTypeOfMaterial() const {
	return 4;
}

/**************** Emissive ****************/
EmissiveMaterial::EmissiveMaterial(const glm::vec3 reflectionCoefficient, const float emissivity)
	: Material(reflectionCoefficient), m_emissivity(emissivity) {}

float EmissiveMaterial::getEmissivity() const {
	return m_emissivity;
}

glm::vec3 EmissiveMaterial::getBRDF(const float wInInclination, const float wInAzimuth,
	const float wOutInclination, const float wOutAzimuth) const {
	return m_rho;
}

int EmissiveMaterial::getTypeOfMaterial() const {
	return 5;
}