// Defines
#define _USE_MATH_DEFINES // for C++

// Includes
#include <chrono>
#include <iostream>
#include <string>
#include <cmath>

#include "include/Scene.h"
#include "include/Camera.h"

int main() {
	// Timer 
	time_t startTime, currTime, startRenderTime;
	time(&startTime);
	std::cout << "Starting the Ray tracer" << std::endl;

	// Variables
	static const int WIDTH = 200;
	static const int HEIGHT = 200;
	const int NR_PHOTON_EMISSION = 5000;
	const int NR_SUBSAMPLES = 2;

	// Create scene
	std::shared_ptr<Scene> scene = Scene::generateScene();

	// Build photon map
	scene->generatePhotonMap(NR_PHOTON_EMISSION);

	time(&startRenderTime);
	double prerenderTime = difftime(startRenderTime, startTime);

	// Create camera
	std::shared_ptr<Camera> camera = std::make_shared<Camera>(
		glm::vec3(0.0f, 0.0f, 3.2f),	// Eye (position of camera)
		//glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f),				// Center (position to look at)
		glm::vec3(0.0f, 1.0f, 0.0f),	// Up vector
		M_PI / 3,						// Field of view in radians
		WIDTH,							// Image pixel width,
		HEIGHT);						// Image pixel height

	// Render scene
	scene->render(camera, NR_SUBSAMPLES);

	time(&currTime);
	// Time for rendering final image
	double timeElapsed	= difftime(currTime, startTime);
	int hoursElapsed	= int(timeElapsed) / (60 * 60);
	int minutesElapsed	= (int(timeElapsed) % (60 * 60)) / 60;
	int secondsElapsed	= int(timeElapsed) % 60;

	std::string renderingTime =
		std::to_string(hoursElapsed)	+ "h:" +
		std::to_string(minutesElapsed)	+ "m:" +
		std::to_string(secondsElapsed)	+ "s";
	std::cout << "Rendering time: " << renderingTime << std::endl;

	///* Time for creation of photon map
	int preRenderHoursElapsed	= int(prerenderTime) / (60 * 60);
	int preRenderMinutesElapsed = (int(prerenderTime) % (60 * 60)) / 60;
	int preRenderSecondsElapsed = int(prerenderTime) % 60;

	std::string prerenderingTime =
		std::to_string(preRenderHoursElapsed)	+ "h:" +
		std::to_string(preRenderMinutesElapsed) + "m:" +
		std::to_string(preRenderSecondsElapsed) + "s";
	std::cout << "Prerendering time: " << prerenderingTime << std::endl;
	//*/

	// Create image
	camera->generateImage();

	return 0;
}