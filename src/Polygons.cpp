#include "Polygons.h"
#include "VertexBufferLayout.h"

namespace Polygons
{
	Vertex3fCol::Vertex3fCol(Vectors::Vector3f pos, Colours::Colour col) : Position(pos), Colour(col) {}
	Vertex3fCol::Vertex3fCol(float x, float y, float z, float r, float g, float b, float a) : Position(x, y, z), Colour(r, g, b, a) {}

	StaticMesh::StaticMesh() {}
	void StaticMesh::NewVertex(Vertex3fCol v)
	{
		Points.push_back(v);
	}
	void StaticMesh::NewQuad(unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3)
	{
		Indexes.push_back(i0);
		Indexes.push_back(i1);
		Indexes.push_back(i2);

		Indexes.push_back(i2);
		Indexes.push_back(i3);
		Indexes.push_back(i0);
	}

	void StaticMesh::ExctractPointData(const float* data, unsigned int length)
	{
		ASSERT(length % 7 == 0);
		for (int i = 0; i < length; i += 7)
		{
			Points.push_back({ data[i], data[i + 1], data[i + 2], data[i + 3], data[i + 4], data[i + 5], data[i + 6] });
		}
	}

	void StaticMesh::ExctractIndexData(unsigned int* data, unsigned int length)
	{
		ASSERT(length % 6 == 0);
		for (int i = 0; i < length; i += 6)
		{
			Indexes.push_back(data[i]);
			Indexes.push_back(data[i + 1]);
			Indexes.push_back(data[i + 2]);
			Indexes.push_back(data[i + 3]);
			Indexes.push_back(data[i + 4]);
			Indexes.push_back(data[i + 5]);
		}
	}

	void StaticMesh::FillBuffers()
	{
		VertexArr = std::make_unique<VertexArray>();
		VertexBuff = std::make_unique<VertexBuffer>(Points.data(), Points.size() * sizeof(Vertex3fCol));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(4);
		VertexArr->AddBuffer(*VertexBuff, layout);
		IndexBuff = std::make_unique<IndexBuffer>(Indexes.data(), Indexes.size());
	}

	DynaMesh::DynaMesh(unsigned int vertCount)
	{
		VertexArr = std::make_unique<VertexArray>();

		VertexBuff = std::make_unique<VertexBuffer>(nullptr, sizeof(Vertex3fCol) * vertCount, GL_DYNAMIC_DRAW);
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(4);

		VertexArr->AddBuffer(*VertexBuff, layout);
	}

	void DynaMesh::NewVertex(Vertex3fCol vertData)
	{
		Points.push_back(vertData);
	}

	void DynaMesh::NewQuad(unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3)
	{
		Indexes.push_back(i0);
		Indexes.push_back(i1);
		Indexes.push_back(i2);

		Indexes.push_back(i2);
		Indexes.push_back(i3);
		Indexes.push_back(i0);
	}

	void DynaMesh::FillBuffers()
	{
		IndexBuff = std::make_unique<IndexBuffer>(Indexes.data(), Indexes.size());
		VertexBuff->SendVertexData(Points.data(), Points.size() * sizeof(Vertex3fCol));
	}

	void DynaMesh::ResetPointData()
	{
		Points.clear();
	}
	void DynaMesh::ResetIndexdata()
	{
		Indexes.clear();
	}
}