#pragma once

#include <glm/glm.hpp>

namespace Engine {

	class TransformObject
	{
	public:
		TransformObject();
		~TransformObject();

		glm::mat4 GetMatrix() const { return m_Matrix; }

		// Get Position
		void SetWorldPosition(glm::vec3 position);
		// Add World Position
		void AddLocalPosition(glm::vec3 transform);

		// Get Rotation
		// Set World Rotation
		// Set Local Rotation
		// Add Local Rotation
		// Add World Rotation

		// Get Scale
		// Add Local Scale
		// Set Local Scale

		// Should probably only be called at the end of the frame
		// TODO: Only call once, in update
		void UpdateMatrix();
	private:
		glm::mat4 m_Matrix{ 1 };
		glm::mat4 m_Position{ 1 };
		glm::mat4 m_Rotation{ 1 };
		glm::mat4 m_Scale{ 1 };
	};

}