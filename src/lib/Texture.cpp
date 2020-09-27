#include "Texture.h"

namespace lib
{
	GLuint Texture::id()const
	{
		return m_id;
	}

	bool Texture::deviceValid()const
	{
		return id();
	}
}