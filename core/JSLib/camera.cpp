#include "camera.h"
#include "transformation.h"

namespace JSLib
{
	ew::Mat4 Camera::ViewMatrix()
	{
		ew::Vec3 up = ew::Vec3(0, 1, 0);
		
		return LookAt(position, target, up);
	}

	ew::Mat4 Camera::ProjectionMatrix()
	{
		if (orthographic)
			return Orthographic(orthoSize, aspectRatio, nearPlane, farPlane);
		else
			return Perspective(fov, aspectRatio, nearPlane, farPlane);
	}
}