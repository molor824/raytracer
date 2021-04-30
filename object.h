#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/norm.hpp>

#define printvec(a) printf("(%lf, %lf, %lf)", (a)[0], (a)[1], (a)[2])
#define printvecln(a) printf("(%lf, %lf, %lf)\n", (a)[0], (a)[1], (a)[2])

typedef glm::dvec3 vec3;
typedef glm::dquat quat;
const int COL_CUBE = 0;
const int COL_SPHERE = 1;

vec3 vec3Slerp(vec3 a, vec3 b, f64 t) {
	auto dot = glm::dot(a, b);
	if(dot > 1) dot = 1;
	if(dot < -1) dot = -1;
	auto theta = acos(dot) * t;
	auto relative = glm::normalize(b - a * dot);
	return a * cos(theta) + relative * sin(theta);
}
struct Object;
struct Mat;
struct Ray {
	vec3 origin, dir;
};
struct Collision {
	vec3 point, normal;
	Mat *hitObjMat;
};
struct Color {
	u8 r, g, b;
	Color lerp(Color other, float t) {
		other.r = mth::lerp(r, other.r, t);
		other.g = mth::lerp(g, other.g, t);
		other.b = mth::lerp(b, other.b, t);
		return other;
	}
};
struct Mat {
	Color col;
	f64 smoothness;
};
struct Object {
	vec3 pos;
	vec3 scale = vec3(1, 1, 1);
	quat rot = glm::identity<quat>();
	i32 colType;
	Mat mat;

	Collision intersectCrnt(Ray ray, double dist = 10000) {
		auto col = Collision();
		auto ray1 = Ray {
			glm::inverse(rot) * (ray.origin - pos) / scale,
			glm::normalize(glm::inverse(rot) * ray.dir / scale),
		};
		if(colType == COL_CUBE) {
		}
		else {
			if(glm::length2(ray1.origin) <= .25) return Collision();

			auto dot = glm::dot(-ray1.origin, ray1.dir);
			auto dotPoint = ray1.dir * dot + ray1.origin;
			auto y = .25 - glm::length2(dotPoint);
			if(y < 0) return Collision();
			col.point = dotPoint - ray1.dir * sqrt(y) * 2.0;
			col.normal = rot * glm::normalize(col.point / scale);
			col.point = rot * (col.point * scale) + pos;
			if(glm::length2(col.point - ray.origin) > dist * dist) return Collision();
			col.hitObjMat = &mat;
		}
		return col;
	}
	static Collision intersect(Ray ray, std::vector<Object> objs, double dist = 10000) {
		auto col = Collision();
		for(auto &obj : objs) {
			auto col1 = obj.intersectCrnt(ray, dist);
			if(!col.hitObjMat || (col1.hitObjMat && glm::length2(col1.point - ray.origin) < glm::length2(col.point - ray.origin)))
				col = col1;
		}
		return col;
	}
};