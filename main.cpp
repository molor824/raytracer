#include <iostream>
#include <fstream>
#include <ctime>
#include <sys/ioctl.h>
#include <unistd.h>
#include "rustTypes.h"
#include "extraMath.h"
#include "object.h"
#include "raytrace.h"

const double angle2rad = M_PI / 180;
const double fps = 1 / 15.0;

i32 main() {
	auto test = Cam();
	test.pos = vec3(0, 0, -4);
	test.objs.resize(2, Object());
	test.objs[0].pos = vec3(.5, 0, 0);
	test.objs[0].colType = COL_SPHERE;
	test.objs[0].mat = Mat{Color{0, 255, 0}, .5};
	test.objs[1].pos = vec3(-.5, 0, 0);
	test.objs[1].colType = COL_SPHERE;
	test.objs[1].mat = Mat{Color{255, 0, 0}, .5};
	winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	test.width = size.ws_col / 2;
	test.height = size.ws_row - 1;
	auto begin = clock();
	i32 i = 0;
	while(i < 360 / 45 / fps) {
		if(clock() - begin > fps * CLOCKS_PER_SEC) {
			begin = clock();
			test.pos = glm::angleAxis(45 * angle2rad * fps, glm::normalize(vec3(.5, 1, 0))) * test.pos;
			test.rot = glm::quatLookAt(glm::normalize(-test.pos), vec3(0, 1, 0));
			test.raytrace();
			test.render();
			i++;
		}
	}
}