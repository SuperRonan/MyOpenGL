#include <voxels/Chunk.h>

Chunk::Chunk(glm::ivec3 size):
	_dims(size),
	_strides(size.y* size.z, size.z, 1),
	_voxels(std::vector<Voxel>(size.x* size.y* size.z)),
	_handle(0)
{}


Chunk::Chunk(Chunk&& other) :
	_dims(std::move(other._dims)),
	_strides(std::move(other._strides)),
	_voxels(std::move(other._voxels)),
	_handle(other._handle)
{
	other._handle = 0;
}

Chunk::~Chunk()
{
	deleteSSBO();
}


glm::ivec3 Chunk::strides()const
{
	return _strides;
}

glm::ivec3 Chunk::dims()const
{
	return _dims;
}

size_t Chunk::size()const
{
	return _dims.x * _dims.y * _dims.z;
}

size_t Chunk::byteSize()const
{
	return size() * sizeof(Voxel);
}

GLuint Chunk::handle()const
{
	return _handle;
}

size_t Chunk::arrayId(glm::ivec3 const& gid)const
{
	return gid.x * _strides.x + gid.y * _strides.y + gid.z * _strides.z;
}

Voxel& Chunk::operator()(glm::ivec3 const& gid)
{
	return _voxels[arrayId(gid)];
}

Voxel const& Chunk::operator()(glm::ivec3 const& gid)const
{
	return _voxels[arrayId(gid)];
}

Voxel& Chunk::operator()(size_t aid)
{
	return _voxels[aid];
}

Voxel const& Chunk::operator()(size_t aid)const
{
	return _voxels[aid];
}

Voxel& Chunk::operator[](size_t aid)
{
	return _voxels[aid];
}

Voxel const& Chunk::operator[](size_t aid)const
{
	return _voxels[aid];
}

void Chunk::createSSBO(bool send_data)
{
	assert(_handle == 0);
	glCreateBuffers(1, &_handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _handle);
	void* ptr = send_data ? _voxels.data() : nullptr;
	glBufferData(GL_SHADER_STORAGE_BUFFER, byteSize(), ptr, GL_DYNAMIC_READ);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Chunk::updateSSBO()
{
	assert(_handle != 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _handle);
	// TODO optimize by sending only the part that needs to be updated
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, byteSize(), _voxels.data());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Chunk::deleteSSBO()
{
	if (_handle)
		glDeleteBuffers(1, &_handle);
	_handle = 0;
}

void Chunk::bind(int offset)
{
	assert(_handle != 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, offset, _handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Chunk::unBind(int offset)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, offset, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

const auto& Chunk::begin()const
{
	return _voxels.begin();
}

const auto& Chunk::end()const
{
	return _voxels.end();
}

const auto& Chunk::cbegin()const
{
	return _voxels.cbegin();
}

const auto& Chunk::cend()const
{
	return _voxels.cend();
}

auto Chunk::begin()
{
	return _voxels.begin();
}

auto Chunk::end()
{
	return _voxels.end();
}