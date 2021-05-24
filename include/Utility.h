#pragma once

#include <random>
#include <algorithm>
#include <cassert>

#include "../external/glm/glm/glm.hpp"
#include "../external/glm/glm/gtc/constants.hpp"
#include "../external/glm/glm/gtx/norm.hpp"
#include "../external/glm/glm/gtx/rotate_vector.hpp"

namespace Utility {
	glm::vec3 NonParallellVector(const glm::vec3& v) {
		if (abs(v.x) < FLT_EPSILON) {
			return glm::vec3(1.0f, 0.0f, 0.0f);
		}
		else if (abs(v.y) < FLT_EPSILON) {
			return glm::vec3(0.0f, 1.0f, 0.0f);
		}
		else if (abs(v.z) < FLT_EPSILON) {
			return glm::vec3(0.0f, 0.0f, 1.0f);
		}
		else {
			// None of v-parameters are zero. 
			return glm::vec3(-v.y, v.z, -v.x);
		}
	}

	glm::vec3 RandomHemishpereSampleDirection(const glm::vec3& n) {
		// Samples uniform angles.
		float incl = (rand() / static_cast<float>(RAND_MAX)) * glm::half_pi<float>();
		float azim = (rand() / static_cast<float>(RAND_MAX)) * glm::two_pi<float>();
		glm::vec3 nonParallellVector = NonParallellVector(n);
		assert(glm::length(glm::cross(nonParallellVector, n)) > FLT_EPSILON);
		glm::vec3 rotationVector = glm::cross(nonParallellVector, n);
		glm::vec3 inclVector = rotate(n, incl, rotationVector);
		return glm::normalize(rotate(inclVector, azim, n));
	}

	glm::vec3 CosineWeightedHemisphereSampleDirection(const glm::vec3& n) {
		// See https://pathtracing.wordpress.com/2011/03/03/cosine-weighted-hemisphere/.
		// Samples cosine weighted positions.
		float r1 = rand() / static_cast<float>(RAND_MAX);
		float r2 = rand() / static_cast<float>(RAND_MAX);

		float theta = acos(sqrt(1.0f - r1));
		float phi = 2.0f * glm::pi<float>() * r2;

		float xs = sinf(theta) * cosf(phi);
		float ys = cosf(theta);
		float zs = sinf(theta) * sinf(phi);

		glm::vec3 y(n.x, n.y, n.z);
		glm::vec3 h = y;
		if (abs(h.x) <= abs(h.y) && abs(h.x) <= abs(h.z)) {
			h.x = 1.0;
		}
		else if (abs(h.y) <= abs(h.x) && abs(h.y) <= abs(h.z)) {
			h.y = 1.0;
		}
		else {
			h.z = 1.0;
		}

		glm::vec3 x = glm::normalize(glm::cross(h, y));
		glm::vec3 z = glm::normalize(glm::cross(x, y));
		return glm::normalize(xs * x + ys * y + zs * z);
	}
}