#pragma once

#include <vector>
#include <glad/glad.h>
#include <cassert>
#include <glm/glm.hpp>



struct Voxel
{
	int32_t id;
};

class Chunk
{
protected:

	glm::ivec3 _dims, _strides;
	std::vector<Voxel> _voxels;
	GLuint _handle;

public:


	Chunk(glm::ivec3 size = { 0, 0, 0 });

	Chunk(Chunk const&) = delete;

	Chunk(Chunk&& other);

	~Chunk();

	glm::ivec3 strides()const;

	glm::ivec3 dims()const;

	size_t size()const;

	size_t byteSize()const;

	GLuint handle()const;

	size_t arrayId(glm::ivec3 const& gid)const;

	Voxel& operator()(glm::ivec3 const& gid);

	Voxel const& operator()(glm::ivec3 const& gid)const;

	Voxel& operator()(size_t aid);

	Voxel const& operator()(size_t aid)const;

	Voxel& operator[](size_t aid);

	Voxel const& operator[](size_t aid)const;

	void createSSBO(bool send_data = false);

	void updateSSBO();

	void deleteSSBO();

	void bind(int offset = 0);

	void unBind(int offset = 0);

	const auto& begin()const;

	const auto& end()const;

	const auto& cbegin()const;

	const auto& cend()const;

	auto begin();

	auto end();
};