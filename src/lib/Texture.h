#pragma once

#include <img/Image.h>
#include <img/Color.h>
#include <glad/glad.h>
#include <cassert>
#include <type_traits>

namespace lib
{
	class Texture
	{
	protected:

		GLuint m_id = 0;

	public:

		virtual ~Texture() {};

		virtual bool hostLoaded()const = 0;
		
		virtual void sendToDevice() = 0;

		virtual void deleteHost() = 0;

		GLuint id()const;
	};

	template <class T>
	class _Texture: public Texture
	{
	protected:

		static_assert(std::is_same<T, img::RGB<unsigned char>>::value);

		using Image = img::Image<T>;

		Image m_img;

	public:

		_Texture() {};

		_Texture(Image const& img) :
			m_img(img)
		{}

		_Texture(_Texture const& other) = delete;

		_Texture(_Texture&& other) = default;


		virtual ~_Texture()override
		{
			if (m_id)
			{
				glDeleteTextures(1, &m_id);
				m_id = 0;
			}
		}


		virtual bool hostLoaded()const override
		{
			return !m_img.empty();
		}

		virtual void sendToDevice()override
		{
			assert(hostLoaded());
			if (m_id == 0)
			{
				glGenTextures(1, &m_id);
				glBindTexture(GL_TEXTURE_2D, m_id);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_img.width(), m_img.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, m_img.data());
				glGenerateMipmap(GL_TEXTURE_2D);
			}
		}

		virtual void deleteHost()override
		{
			m_img = Image();
		}
	};
}