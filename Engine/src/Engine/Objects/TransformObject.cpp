#include "epch.h"
#include "TransformObject.h"
#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtx/matrix_decompose.hpp>

namespace Engine {
	TransformObject::TransformObject()
	{
	}
	TransformObject::~TransformObject()
	{
	}
	void TransformObject::UpdateMatrix()
	{
		m_Matrix = m_Scale * m_Rotation * m_Position;
	}
	void TransformObject::Reset()
	{
		m_Matrix	= glm::mat4(1.f);

		m_Position	= glm::mat4(1.f);
		m_Rotation	= glm::mat4(1.f);
		m_Scale		= glm::mat4(1.f);
	}

	/* Position */
	void TransformObject::SetWorldPosition(glm::vec3 position)
	{
		m_Position = glm::translate(glm::inverse(m_Rotation) * glm::mat4(1.f), position) * m_Rotation;
	}
	void TransformObject::AddWorldPosition(glm::vec3 transform)
	{
		glm::vec3 currentPosition(m_Matrix[3]);
		glm::vec3 travel = transform - currentPosition;
		m_Position = glm::translate(glm::inverse(m_Rotation) * m_Position, travel) * m_Rotation;
	}
	//void TransformObject::AddLocalPosition(glm::vec3 transform)
	//{
	//	m_Position = glm::translate(glm::mat4(1.f), transform);
	//}
	
	/* Rotation */
	void TransformObject::AddLocalRotation(float radians, glm::vec3 rotationAxis)
	{
		m_Rotation = glm::rotate(m_Rotation, radians, rotationAxis);
	}
	void TransformObject::AddWorldRotation(float radians, glm::vec3 rotationAxis)
	{
		m_Rotation *= glm::inverse(m_Rotation) * glm::rotate(glm::mat4(1.f), radians, rotationAxis) * m_Rotation;
	}

	/* Scale */
	void TransformObject::AddLocalScale(float scale, glm::vec3 scaleAxis)
	{
		m_Scale = glm::scale(m_Scale, glm::vec3(1.f) + (scaleAxis * scale));
	}
	//void TransformObject::AddLocalScale(glm::vec3 scaleAxis)
	//{
	//	m_Scale = glm::scale(m_Scale, glm::vec3(1.f) + (scaleAxis));
	//}
	//void TransformObject::SetScale(glm::vec3 scale)
	//{
	//	m_Scale = glm::scale(m_Scale, scale);
	//}



}