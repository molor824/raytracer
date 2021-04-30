#pragma once

#include <thread>

struct Cam {
	vec3 pos;
	quat rot = glm::identity<quat>();
	f64 fov = .5;
	f64 dist = 10000;
	i32 width, height;
	std::vector<Object> objs;
	i32 threads = 4;
	i32 maxBounce = 4;
	std::vector<Color> cols;
	Color sky = Color{0, 127, 255};

	static void raytracePerThread(Cam* self, int crntThread) {
		for(i32 y = 0; y < self->height; y++) {
			for(i32 x = crntThread; x < self->width; x += self->threads) {
				auto &colr = self->cols[x + y * self->width];
				colr = self->sky;
				auto dir = self->rot * glm::normalize(vec3((x + .5 - self->width / 2.0) / self->height * self->fov,
					(y + .5 - self->height / 2.0) / self->height * self->fov, 1));
				auto col = Object::intersect(Ray{self->pos, dir}, self->objs, self->dist);
				if(col.hitObjMat) {
					std::vector<Mat> hitMats;
					hitMats.push_back(*col.hitObjMat);
					for(i32 i = 0; i < self->maxBounce; i++) {
						dir = vec3Slerp(-dir, col.normal, 2);
						col = Object::intersect(Ray{col.point, dir}, self->objs, self->dist);
						if(!col.hitObjMat) break;
						hitMats.push_back(*col.hitObjMat);
					}
					for(i32 i = hitMats.size() - 1; i >= 0; i--) {
						colr = hitMats[i].col.lerp(colr, hitMats[i].smoothness);
					}
				}
			}
		}
	}
	void raytrace() {
		cols.resize(width * height);

		std::vector<std::thread> handle;
		for(i32 i = 1; i < threads; i++) {
			handle.push_back(std::thread(Cam::raytracePerThread, this, i));
		}
		Cam::raytracePerThread(this, 0);
		for(i32 i = 0; i < threads - 1; i++) {
			handle[i].join();
		}
	}
	void render() {
		int i = 0;
		for(auto c : cols) {
			printf("\x1b[38;2;%d;%d;%dm\u2588\u2588", c.r, c.g, c.b);
			if(i % width == width - 1) printf("\x1b[0m\n");
			i++;
		}
		fflush(stdout);
	}
};