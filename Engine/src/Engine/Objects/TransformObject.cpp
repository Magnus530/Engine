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
		m_Rotation = m_WorldRotation * m_LocalRotation;

		m_Matrix = m_Scale * m_Rotation * m_Position;
	}
	void TransformObject::Reset()
	{
		m_Matrix	= glm::mat4(1.f);

		m_Position	= glm::mat4(1.f);

		m_WorldRotation	= glm::mat4(1.f);
		m_LocalRotation	= glm::mat4(1.f);
		m_Rotation	= glm::mat4(1.f);

		m_Scale		= glm::mat4(1.f);
	}

	/* Position */
	void TransformObject::SetWorldPosition(glm::vec3 position)
	{
		glm::vec3 currentPosition(m_Position[3]);
		//glm::vec3 currentPosition(m_Matrix[3]);
		glm::vec3 travel = position - currentPosition;
		//AddLocalPosition(travel);
		AddWorldPosition(travel);
	}
	void TransformObject::AddWorldPosition(glm::vec3 transform)
	{
		//m_Position = glm::translate(glm::mat4(1.f), transform) * m_Position;
		m_Position = glm::translate(glm::mat4(1.f), transform);
		//m_Matrix += glm::translate(glm::mat4(1.f), transform);
	}
	void TransformObject::AddLocalPosition(glm::vec3 transform)
	{
		m_Position = glm::translate(m_Position, transform);
		//m_Matrix = glm::translate(m_Matrix, transform);
	}
	
	/* Rotation */
	void TransformObject::AddLocalRotation(float radians, glm::vec3 rotationAxis)
	{
		m_LocalRotation = glm::rotate(m_LocalRotation, radians, rotationAxis) * m_Rotation;
	}
	void TransformObject::AddWorldRotation(float radians, glm::vec3 rotationAxis)
	{
		m_Matrix = glm::rotate(m_Matrix, radians, rotationAxis);

		//m_WorldRotation = glm::rotate(glm::mat4(1.f), radians, rotationAxis) * m_WorldRotation;
	}

	/* Scale */
	void TransformObject::AddLocalScale(float scale, glm::vec3 scaleAxis)
	{
		m_Scale = glm::scale(m_Scale, glm::vec3(1.f) + (scaleAxis * scale));
	}
	void TransformObject::AddLocalScale(glm::vec3 scaleAxis)
	{
		m_Scale = glm::scale(m_Scale, glm::vec3(1.f) + (scaleAxis));
	}
	void TransformObject::SetScale(glm::vec3 scale)
	{
		m_Scale = glm::scale(m_Scale, scale);
	}



}