#pragma once

#include "external/glm/glm/glm.hpp"
#include "external/glm/glm/gtc/constants.hpp"

static const float EPSILON = 0.00001f;

// Abstract base class for various materials
class Material {
public:
	// BRDF takes incoming light direction, wIn, and outgoing direction, wOut. 
	// Returns the ratio of reflected radiance exiting along wOut and the 
	// irradiance incident on the surface from wIn. 
	virtual glm::vec3 getBRDF(const float wInInclination, const float wInAzimuth,
		const float wOutInclination, const float wOutAzimuth) const = 0;

	virtual int getTypeOfMaterial() const = 0;

	float getRefractionIndex() const;

protected:
	Material();
	explicit Material(float refractionIndex);
	explicit Material(glm::vec3 reflectionCoefficient);

	glm::vec3 m_rho;
	glm::vec3 m_rhoOverPi; // Albedo
	float m_refractionIndex;
};

/**************** Lambertian ****************/
class LambertianMaterial : public Material {
public:
	explicit LambertianMaterial(const glm::vec3 reflectionCoefficient);

	glm::vec3 getBRDF(const float wInInclination, const float wInAzimuth,
		const float wOutInclination, const float wOutAzimuth) const override;

	int getTypeOfMaterial() const override;
};

/**************** Oren-Nayar ****************/
class OrenNayarMaterial : public Material {
public:
	explicit OrenNayarMaterial(const glm::vec3 reflectionCoefficient, const float roughness); // Roughness = standard gaussian deviation

	glm::vec3 getBRDF(const float wInInclination, const float wInAzimuth,
		const float wOutInclination, const float wOutAzimuth) const override;

	int getTypeOfMaterial() const override;
private:
	float m_roughness; // Sigma
};

/**************** PerfectReflector ****************/
class PerfectReflectorMaterial : public Material {
public:
	explicit PerfectReflectorMaterial(); 

	glm::vec3 getBRDF(const float wInInclination, const float wInAzimuth,
		const float wOutInclination, const float wOutAzimuth) const override;

	int getTypeOfMaterial() const override;
};

/**************** Transparent ****************/
class TransparentMaterial : public Material {
public:
	explicit TransparentMaterial(const float refractionIndex);

	glm::vec3 getBRDF(const float wInInclination, const float wInAzimuth,
		const float wOutInclination, const float wOutAzimuth) const override;

	int getTypeOfMaterial() const override;
};


/**************** Emissive ****************/
class EmissiveMaterial : public Material {
public:
	explicit EmissiveMaterial(const glm::vec3 reflectionCoefficient, const float emissivity); // emissivity = flux = light emittance

	float getEmissivity() const;
	glm::vec3 getBRDF(const float wInInclination, const float wInAzimuth,
		const float wOutInclination, const float wOutAzimuth) const override;

	int getTypeOfMaterial() const override;
private:
	float m_emissivity;
};
