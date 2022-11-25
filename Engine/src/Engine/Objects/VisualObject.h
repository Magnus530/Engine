//#pragma once
//
//#include "vertex.h"
//#include <vector>
//#include "TransformObject.h"
//#include "../Renderer/VertexArray.h"
//#include "../Renderer/Buffer.h"
//
//
//namespace Engine {
//
//	class VisualObject : public TransformObject
//	{
//	public:
//		VisualObject(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
//		~VisualObject();
//
//		// Get functions
//		std::shared_ptr<VertexArray> GetVertexArray() const { return m_VA; }
//
//	private:
//		std::shared_ptr<VertexArray> m_VA;
//		std::shared_ptr<VertexBuffer> m_VB;
//		std::shared_ptr<IndexBuffer> m_IB;
//	};
//}