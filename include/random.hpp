#pragma once

#include <random>


class Random {
public:
	Random(const Random&) = delete;

	static Random& get() {
		static Random instance;
		return instance;
	}

	static float getRange(float min, float max) {
		return get().IgetRange(min, max);
	}

	static int getRangeInt(int min, int max) {
		return get().IgetRangeInt(min, max);
	}

	template<class T>
	static void shuffle(std::vector<T>& vector) {
		return get().Ishuffle(vector);
	}

private:
	Random() { rng = std::mt19937(dev()); }

	float IgetRange(float min, float max) {
		std::uniform_real_distribution<> dist(min, max);
		return dist(rng);
	}

	float IgetRangeInt(int min, int max) {
		std::uniform_int_distribution<> dist(min, max);
		return dist(rng);
	}

	template<class T>
	void Ishuffle(std::vector<T>& vector) {
		std::ranges::shuffle(vector, rng);
	}

	std::random_device dev;
	std::mt19937 rng;

};