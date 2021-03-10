#pragma once
#ifndef PHYSICS_H
#define PHYSICS_H
#include <vector>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
const float GRAVITY = 100, FRICTION = 0.25, NFRICTION = 1-FRICTION, F_COEF = 0.85;
template<class T> bool AABB(T ax, T ay, T aw, T ah, T bx, T by, T bw, T bh) {
	if (ax > bx + bw)
		return false;
	if (bx > ax + aw)
		return false;
	if (ay > by + bh)
		return false;
	if (by > ay + ah)
		return false;
	return true;
}
template<class T> bool AABB(T a, T ab, T b, T bb) {
	if (a.x > b.x + bb.x)
		return false;
	if (b.x > a.x + ab.x)
		return false;
	if (a.y > b.y + bb.y)
		return false;
	if (b.y > a.y + ab.y)
		return false;
	return true;
}
template<class T> bool AABB(T a, T b) {
	if (a.x > b.x + b.z)
		return false;
	if (b.x > a.x + a.z)
		return false;
	if (a.y > b.y + b.w)
		return false;
	if (b.y > a.y + a.w)
		return false;
	return true;
}
template<class T> bool AAABBB(T a, T ab, T b, T bb) {
	if (a.x > b.x + bb.x)
		return false;
	if (b.x > a.x + ab.x)
		return false;
	if (a.y > b.y + bb.y)
		return false;
	if (b.y > a.y + ab.y)
		return false;
	if (a.z > b.z + bb.z)
		return false;
	if (b.z > a.z + ab.z)
		return false;
	return true;
}

template <class T, class S> class Field {
public:
	std::vector<T> force_;
	std::vector<std::string> force_source;
	std::vector<S> force;

	std::vector<T> weight_;
	std::vector<std::string> weight_source;
	std::vector<double> weight;

	Field() {
		//force = std::map<T, S>();
		force_ = std::vector<T>();
		force_source = std::vector<std::string>();
		force = std::vector<S>();

		weight_ = std::vector<T>();
		weight_source = std::vector<std::string>();
		weight = std::vector<double>();
	}

	void addForce(T key, S value, std::string source) {
		force_.push_back(key);
		force.push_back(value);
		force_source.push_back(source);
	}

	void clearForces(std::string source) {
		for (int i = 0; i < force_source.size(); i++)
			if (force_source[i] == source) {
				force.erase(force.begin() + i);
				force_.erase(force_.begin() + i);
				force_source.erase(force_source.begin() + i);
			}
	}

	S getForce(T key) {
		S out = S();
		for (int i = 0; i < force.size(); i++) {
			if (AABB(force_[i], key))
				out += force[i];
		}
		return out;
	}
	std::vector<int> getForceIndex(T key) {
		std::vector<int> out = std::vector<int>;
		for (int i = 0; i < force.size(); i++)
			if (AABB(force_[i], key))
				out.push_back(i);
		return out;
	}
	std::vector<std::string> getForceSource(T key) {
		std::vector<std::string> out = std::vector<std::string>();
		for (int i = 0; i < force.size(); i++)
			if (AABB(force_[i], key))
				out.push_back(force_source[i]);
		return out;
	}

	void addWeight(T key, double value, std::string source) {
		weight_.push_back(key);
		weight.push_back(value);
		weight_source.push_back(source);
	}

	void clearWeights(std::string source) {
		for (int i = 0; i < weight_source.size(); i++)
			if (weight_source[i] == source) {
				weight.erase(weight.begin() + i);
				weight_.erase(weight_.begin() + i);
				weight_source.erase(weight_source.begin() + i);
			}
	}

	double getWeight(T key) {
		double out = double();
		for (int i = 0; i < weight.size(); i++) {
			if (AABB(weight_[i], key))
				out += weight[i];
		}
		return out;
	}
	std::vector<int> getWeightIndex(T key) {
		std::vector<int> out = std::vector<int>;
		for (int i = 0; i < weight.size(); i++)
			if (AABB(weight_[i], key))
				out.push_back(i);
		return out;
	}
	std::vector<std::string> getWeightSource(T key) {
		std::vector<std::string> out = std::vector<std::string>();
		for (int i = 0; i < weight.size(); i++)
			if (AABB(weight_[i], key))
				out.push_back(weight_source[i]);
		return out;
	}
};

class P {
public:
	static Field<glm::vec4, glm::vec2> field;
};

#endif