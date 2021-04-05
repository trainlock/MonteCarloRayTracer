#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>

#include "external/glm/glm/glm.hpp"
#include "external/glm/glm/ext.hpp"

#include "Ray.h"

class Camera {
public:
	/*
	enum class ImageResolution {
		RESOLUTION_480p,
		RESOLUTION_720p,
		RESOLUTION_1080p
	};
	*/
	// Constructor
	explicit Camera(glm::vec3 eye, glm::vec3 center, glm::vec3 up, float fov,
		const int pixelWidht, const int pixelHeight // ImageResolution imageResolution
	);

	// Getters
	int getPixelWidth() const;
	int getPixelHeight() const;

	// Cast ray from pixel [x, y] with some randomness in range [-0.5, 0.5] for subsampling
	std::shared_ptr<Ray> castCameraRay(
		const int x,		// [0, width - 1]
		const int y,		// [0, height - 1]
		const float randX,	// [-0.5, 0.5]
		const float randY); // [-0.5, 0.5]

	// Set value of pixel [x, y] in m_pixels to the given value
 	void setPixelValues(const int x, const int y, const glm::vec3 pixelValue);

	// Create a .ppm image from m_pixels
	void generateImage();

private:
	// Image resolution
	int m_pixelWidth;
	int m_pixelHeight;

	// Field of view
	float m_fov;

	// Position and direciton of camera
	glm::vec3 m_eye, m_center, m_up;

	// View and perspective matrix
	glm::mat4 m_invVP;

	// Image pixels
	std::vector<std::vector<glm::vec3>> m_pixels;
};
