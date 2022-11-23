#include "epch.h"
#include "TransformObject.h"
#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtx/matrix_decompose.hpp>

namespace Engine {
	Engine::TransformObject::TransformObject()
	{
	}
	TransformObject::~TransformObject()
	{
	}
	void TransformObject::SetWorldPosition(glm::vec3 position)
	{
		glm::vec3 currentPosition(m_Position[3]);
		glm::vec3 travel = position - currentPosition;
		AddLocalPosition(travel);
		/* Maybe reduntant */
		UpdateMatrix();
	}
	void TransformObject::AddLocalPosition(glm::vec3 transform)
	{
		m_Position = glm::translate(m_Position, transform);
		UpdateMatrix();
	}
	void TransformObject::UpdateMatrix()
	{
		m_Matrix = m_Scale * m_Rotation * m_Position;
	}
}