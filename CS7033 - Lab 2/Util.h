#ifndef UTIL_H
#define UTIL_H

#define MODEL_FOLDER std::string("resources/models/")
#define LEVEL_FOLDER std::string("resources/levels/")

/* Player Action Results */
#define PROJECTILE_SPEED_SLOW 2.0f
#define PROJECTILE_GRAVITY -0.005f
#define PROJECTILE_MAX_DISTANCE 1000.0f
#define PROJECTILE_MODEL_FILE "projectile_MODEL.dae"
#define ACTION_NONE 0
#define ACTION_FIRE_PROJECTILE_SLOW 1

#include <glm.hpp>
#include "Logger.h"

class Util{
public:
	static bool lineTriangleIntersection(glm::vec3 p0, glm::vec3 dir, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2){
		LOGGER_ENTER("Util", "lineTriangleIntersection");
		/* Get edge vectors and normal */
		glm::vec3 e0 = v1 - v0;
		glm::vec3 e1 = v2 - v0;
		glm::vec3 n = glm::cross(e0, e1);

		/* Destination point */
		glm::vec3 p1 = p0 + dir;


		glm::vec3 u = p1 - p0;
		glm::vec3 w = p0 - v1;

		float D = glm::dot(n, u);
		float N = -glm::dot(n, w);

		/* Check if segment is parallel to plane */
		if(glm::abs(D) < 0.00001)
			return false;

		float sI = N / D;
		if(sI < 0 || sI > 1)
			return false;

		glm::vec3 poi = p0 + sI * u;

		float uu = glm::dot(e0, e0);
		float uv = glm::dot(e0, e1);
		float vv = glm::dot(e1, e1);
		glm::vec3 x = poi - v0;
		float xu = glm::dot(x, e0);
		float xv = glm::dot(x, e1);
		float D2 = uv * uv - uu *vv;

		float s = (uv * xv - vv * xu) / D2;
		if(s < 0 || s > 1)
			return false;

		float t = (uv * xu - uu * xv) / D2;
		if(t < 0 || (s + t) > 1)
			return false;
		return true;
		LOGGER_EXIT;
	}
};

#endif