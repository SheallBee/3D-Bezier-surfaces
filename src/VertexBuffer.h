#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererId;
public:
	VertexBuffer(const void* data, unsigned int size);
	VertexBuffer(const void* data, unsigned int size, unsigned int type);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
	void SendVertexData(const void* data, unsigned int size);
};
