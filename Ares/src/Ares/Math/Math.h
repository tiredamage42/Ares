#pragma once
#define GLM_FORCE_RADIANS
//#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/quaternion.hpp>

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <tuple>
namespace Ares
{
    typedef glm::mat4 Matrix4;
    typedef glm::vec4 Vector4;
    typedef glm::vec3 Vector3;
    typedef glm::vec2 Vector2;
    typedef glm::quat Quaternion;

	class Math
	{
	public:
        static std::tuple<Vector3, Quaternion, Vector3> GetTransformDecomposition(const Matrix4& transform)
        {
            Vector3 scale, translation, skew;
            Vector4 perspective;
            Quaternion rotation;
            glm::decompose(transform, scale, rotation, translation, skew, perspective);
            //rotation = glm::conjugate(rotation);
            return { translation, rotation, scale };
        }
	};
}