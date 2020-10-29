#include "../include/SquareLight.h"

SquareLight::SquareLight() : Light() {}

SquareLight::~SquareLight() {}

SquareLight::SquareLight(Vec3f s, double p, double w, Vec3f n, Vec3f r) : Light(s, p) {
	size = w;
	normal = normalize(n);
	right = normalize(r);
}

Vec3f SquareLight::getSource() //por ahora se asume siempre paralela a los ejes
{
	//valores random entre -size/2 y +size/2
	double random_u = (rand() % 1000) / 1000.0;
	double random_r = (rand() % 1000) / 1000.0;
	random_u = (random_u * size) - (0.5f * size);
	random_r = (random_r * size) - (0.5f * size);

	//vector ortogonal a normal y right
	Vec3f up = cross(normal, right);
	return source + up * random_u + right * random_r;
}

Vec3f SquareLight::randomDir(int* seed) {
	double nums[3];
	Vec3f dir = Vec3f(0.f);
	do {
		niederreiter2(3, seed, nums);
		dir.x = (nums[0] * 2) + 1;
		dir.y = (nums[1] * 2) + 1;
		dir.z = (nums[2] * 2) + 1;
	} while (distance(dir, Vec3f(0.f)) > 1.0 || dot(dir, normal) <= 0.6); //puntos en la semiesfera que esta sobre el plano de la luz
	return normalize(dir);
}

Vec3f SquareLight::getNormal() {
	return normal;
}

std::vector<Vec3f> SquareLight::samplePositions() {
	std::vector<Vec3f> samples;
	Vec3f up = cross(normal, right);

	int num_samples = 16;
	int div = num_samples * 2;
	float x = -(num_samples - 1) * size / div;
	float y = (static_cast<__int64>(num_samples) - 1) * size / div;

	while (x <= (static_cast<__int64>(num_samples) - 1) * size / div) {
		while (y >= -(num_samples - 1) * size / div) {
			Vec3f sample = source + up * y + right * x;
			samples.push_back(sample);
			y -= 2 * size / div;
		}
		y = (static_cast<__int64>(num_samples) - 1) * size / div;
		x += 2 * size / div;
	}

	return samples;
}