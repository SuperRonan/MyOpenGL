#pragma once

#include <img/Image.h>
#include <img/Color.h>
#include <glad/glad.h>
#include <cassert>
#include <type_traits>
#include <filesystem>
#include <img/ImRead.h>

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

		bool deviceValid()const;

		virtual void use(GLuint index=0) = 0;
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

		_Texture(std::filesystem::path const& path):
			m_img(img::io::read<T>(path))
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
			if (m_id)
			{
				glDeleteTextures(1, &m_id);
			}

			glGenTextures(1, &m_id);
			glBindTexture(GL_TEXTURE_2D, id());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_img.width(), m_img.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, m_img.data());
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		virtual void deleteHost()override
		{
			m_img = Image();
		}

		virtual void use(GLuint index=0)override
		{
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_2D, id());
		}
	};
}