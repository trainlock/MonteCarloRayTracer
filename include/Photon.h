#pragma once

#ifndef PHOTON_H
#define PHOTON_H

#include "../external/glm/glm/glm.hpp"

const static float PHOTON_RADIUS = 0.05f;

/**************** Photon ****************/
struct Photon {
	glm::vec3 m_position;
	glm::vec3 m_direction;
	glm::vec3 m_flux;	// Watt
	const float RADIUS = PHOTON_RADIUS;

	Photon& operator=(const Photon& p) {
		this->m_position = p.m_position;
		this->m_direction = p.m_direction;
		this->m_flux = p.m_flux;
		return *this;
	}
};

/**************** KDtree node ****************/
struct KDTreeNode {
	typedef double value_type;
	Photon p;
	size_t index;

	// Used in kdtree++ implementation
	value_type operator[](size_t n) const {
		return p.m_position[n];
	}

	// Used in kdtree++ implementation
	double distance(const KDTreeNode& node) {
		return glm::length(node.p.m_position - p.m_position);
	}
};

#endif // PHOTON_H
