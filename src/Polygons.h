#pragma once
#include "Colour.h"
#include "Vectors.h"
#include <unordered_map>
#include "VertexBuffer.h"
#include <memory>
#include <array>
#include "VertexArray.h"
#include "IndexBuffer.h"

namespace Polygons
{
	class Vertex3fCol
	{
	public:
		Vectors::Vector3f Position;
		Colours::Colour Colour;
		Vertex3fCol(Vectors::Vector3f pos, Colours::Colour col);
		Vertex3fCol(float x, float y, float z, float r, float g, float b, float a);
	};
	class StaticMesh
	{
	private:
		std::vector<Vertex3fCol> Points;
		std::vector<unsigned int> Indexes;
	public:
		std::unique_ptr<VertexBuffer> VertexBuff;
		std::unique_ptr<IndexBuffer> IndexBuff;
		std::unique_ptr<VertexArray> VertexArr;

		StaticMesh();
		//~StaticMesh();
		void NewVertex(Vertex3fCol v);
		void NewQuad(unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3);
		void ExctractPointData(const float* data, unsigned int length);
		void ExctractIndexData(unsigned int* data, unsigned int length);
		void FillBuffers();
	};

	class DynaMesh
	{
	public:
		std::vector<Vertex3fCol> Points;
		std::vector<unsigned int> Indexes;

		std::unique_ptr<VertexBuffer> VertexBuff;
		std::unique_ptr<IndexBuffer> IndexBuff;
		std::unique_ptr<VertexArray> VertexArr;

		DynaMesh(unsigned int vertCount);
		//~StaticMesh();
		void NewVertex(Vertex3fCol vertData);
		void NewQuad(unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3);
		void FillBuffers();
		void ResetPointData();
		void ResetIndexdata();
	};
}