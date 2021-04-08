#include "../include/Camera.h"

Camera::Camera(glm::vec3 eye, glm::vec3 center, glm::vec3 up, float fov,
	const int pixelWidht, const int pixelHeight // ImageResolution imageResolution)
) : m_eye(eye), m_center(center), m_up(up), m_fov(fov), 
	m_pixelWidth(pixelWidht), m_pixelHeight(pixelHeight) {

	// View and perspective matrices
	glm::mat4 V = glm::lookAt(eye, center, up);
	float aspect = (float)m_pixelWidth / (float)m_pixelHeight;
	glm::mat4 P = glm::perspective(fov, aspect, 0.1f, 100.0f);
	m_invVP = glm::inverse(V * P);

	// Setup pixel data
	m_pixels.resize(m_pixelHeight);
	for (int i = 0; i < m_pixelHeight; i++) m_pixels[i].resize(m_pixelWidth);
}

int Camera::getPixelHeight() const {
	return m_pixelHeight;
}

int Camera::getPixelWidth() const {
	return m_pixelWidth;
}

std::shared_ptr<Ray> Camera::castCameraRay(const int x, const int y, const float randX, const float randY) {
	// Check if input is valid
	if (x < 0 || x > m_pixelWidth - 1 ||
		y < 0 || y > m_pixelHeight - 1 ||
		randX < -0.5f || randX > 0.5f ||
		randY < -0.5f || randY > 0.5f) {
		std::cout << "Error: Invalid arguments to Camera::castCameraRay()" << std::endl;
		return std::make_shared<Ray>(glm::vec3(0.0f), glm::vec3(0.0f));
	}
	else {
		// Find direction out of the frustum depending on which pixel the ray is shot through
		// From is on the near-viewplane and To is on the far-viewplane
		glm::vec4 from4 = m_invVP * glm::vec4(
			(((float)x + randX) / (float)m_pixelWidth - 0.5f) * 2.0f,
			(((float)y + randY) / (float)m_pixelHeight - 0.5f) * 2.0f,
			1.0f, 1.0f);
		glm::vec4 to4 = m_invVP * glm::vec4(
			(((float)x + randX) / (float)m_pixelWidth - 0.5f) * 2.0f,
			(((float)y + randY) / (float)m_pixelHeight - 0.5f) * 2.0f,
			-1.0f, 1.0f);

		glm::vec3 from = glm::vec3(from4) * from4.w;
		glm::vec3 to = glm::vec3(to4) * to4.w;

		// Calculate the direction of the ray
		glm::vec3 direction = glm::normalize(to - from); 

		return std::make_shared<Ray>(m_eye, direction);
	}
}

void Camera::setPixelValues(const int x, const int y, const glm::vec3 pixelValue) {
	// Check that input value is valid
	if (x < 0 || x >= m_pixelWidth ||
		y < 0 || y >= m_pixelHeight) return;

	m_pixels[x][y] = pixelValue;
}

void Camera::generateImage() {
	// Create .ppm image from values in m_pixels
	std::cout << "Writing to PPM..." << std::endl;

	// Check dimension of image
	if (m_pixelHeight == 0 || m_pixelWidth == 0) {
		fprintf(stderr, "Can't save an empty image\n");
		exit(1);
	}

	FILE* fp;
	fopen_s(&fp, "data/output/test.ppm", "wb"); // b = binary mode
	//fopen_s(&fp, path.c_str(), "wb"); // b = binary mode
	std::cout << "Opened file" << std::endl;

	// Check if file was loaded
	if (!fp) {
		fprintf(stderr, "Failed to load ppm-file\n");
		exit(1);
	}

	// Write ppm-header
	std::fprintf(fp, "P6\n%d %d\n255\n", m_pixelWidth, m_pixelHeight);

	// Gamma correction
	// Write pixel info to ppm-file
	for (int j = 0; j < m_pixelWidth; j++) {
		for (int i = 0; i < m_pixelHeight; i++) {
			glm::vec3 pixel = m_pixels[i][j] * glm::vec3(255.0f);

			std::fputc((int)pixel.r, fp);
			std::fputc((int)pixel.g, fp);
			std::fputc((int)pixel.b, fp);
		}
	}
	std::cout << "Finished writing to file" << std::endl;

	// Close file
	std::fclose(fp);
}

void Camera::generateImage(const std::string fileName, bool createTGA) {
	const std::string path = "output/" + path + ".ppm";
	createPPMImage(path);
	/*
	//const std::string path = "output/" + path + ".tga";
	if (createTGA) {
		const std::string path = "output/" + path + ".tga";
		//createTGAImage(path);
	}
	else {
		const std::string path = "output/" + path + ".ppm";
		createPPMImage(path);
	}
	*/
}

void Camera::createPPMImage(const std::string path) {
	// Create .ppm image from values in m_pixels
	std::cout << "Writing to PPM..." << std::endl;

	// Check dimension of image
	if (m_pixelHeight == 0 || m_pixelWidth == 0) {
		fprintf(stderr, "Can't save an empty image\n");
		exit(1);
	}

	FILE* fp;
	//fopen_s(&fp, "test.ppm", "wb"); // b = binary mode
	fopen_s(&fp, path.c_str(), "wb"); // b = binary mode
	std::cout << "Opened file" << std::endl;

	// Check if file was loaded
	if (!fp) {
		fprintf(stderr, "Failed to load ppm-file\n");
		exit(1);
	}

	// Write ppm-header
	std::fprintf(fp, "P6\n%d %d\n255\n", m_pixelWidth, m_pixelHeight);

	// Gamma correction
	// Write pixel info to ppm-file
	for (int j = 0; j < m_pixelWidth; j++) {
		for (int i = 0; i < m_pixelHeight; i++) {
			glm::vec3 pixel = m_pixels[i][j] * glm::vec3(255.0f);

			std::fputc((int)pixel.r, fp);
			std::fputc((int)pixel.g, fp);
			std::fputc((int)pixel.b, fp);
		}
	}
	std::cout << "Finished writing to file" << std::endl;

	// Close file
	std::fclose(fp);
}
/*
void Camera::createTGAImage(const std::string path) {
	std::cout << "Writing to TGA..." << std::endl;
	assert(m_pixelWidth > 0 && m_pixelHeight > 0);
	const std::string path = "test.tga";
	std::ofstream o(path.c_str(), std::ios::out | std::ios::binary);

	// Write file header
	const std::string header = "002000000000";
	for (unsigned int i = 0; i < header.length(); ++i) {
		o.put(header[i] - '0');
	}

	o.put(m_pixelWidth & 0x00FF);
	o.put((m_pixelWidth & 0xFF00) >> 8);
	o.put(m_pixelWidth & 0x00FF);
	o.put((m_pixelHeight & 0xFF00) >> 8);
	o.put(32); // 24 bit bitmap.
	o.put(0);

	// Write data to image
	for (int y = 0; y < m_pixelHeight; ++y) {
		for (int x = 0; x < m_pixelWidth; ++x) {
			glm::vec3 pixel = m_pixels[x][y] * glm::vec3(255.0f);
			auto& cp = pixel;
			o.put(cp.b);
			o.put(cp.g);
			o.put(cp.r);
			o.put((char)0xFF); // 255.
		}
	}

	// Close file
	o.close();
}
*/