//#pragma once
//
//#include <glm/glm.hpp>
//
//
//// This class is decrepit, to be replaced with an entity system 
//namespace Engine {
//
//	class TransformObject
//	{
//	public:
//		TransformObject();
//		~TransformObject();
//		void UpdateMatrix();
//		void Reset();
//
//		glm::mat4 GetMatrix() const { return m_Matrix; }
//		glm::vec3 GetPosition() const { return m_Matrix[3]; }
//
//		void SetWorldPosition(glm::vec3 position);
//		void AddWorldPosition(glm::vec3 transform);
//		//void AddLocalPosition(glm::vec3 localTransform);
//
//		// Get Rotation
//		// Set World Rotation
//		// Set Local Rotation
//		void AddLocalRotation(float radians, glm::vec3 rotationAxis);
//		void AddWorldRotation(float radians, glm::vec3 rotationAxis);
//
//		// Get Scale
//		void AddLocalScale(float scale, glm::vec3 scaleAxis);
//		//void AddLocalScale(glm::vec3 scaleAxis);
//		//void SetScale(glm::vec3 scale);
//
//		// Should probably only be called at the end of the frame
//		// TODO: Only call once, in update
//	private:
//		glm::mat4 m_Matrix{ 1 };
//
//		glm::mat4 m_Position{ 1 };
//
//		//glm::mat4 m_WorldRotation{ 1 };
//		//glm::mat4 m_LocalRotation{ 1 };
//		glm::mat4 m_Rotation{ 1 };
//
//		glm::mat4 m_Scale{ 1 };
//	};
//
//}