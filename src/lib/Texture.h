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

		using Image = img::Image<T>;

		Image m_img;

		static constexpr GLint GL_PIXEL_TYPE()
		{
			if constexpr (((img::is_RGB<T>::value || img::is_RGBA<T>::value) && std::is_same<T::_Type, unsigned char>::value) || std::is_same<T, unsigned char>::value)
			{
				return GL_UNSIGNED_BYTE;
			}
			// TODO static assert
			assert(false, std::string("Unsuported texture pixel type for OpenGL: ") + typeid(T).name());
		}

		static constexpr GLint GL_PIXEL_FORMAT()
		{
			if constexpr (img::is_RGB<T>::value)
			{
				return GL_RGB;
			}
			if constexpr (img::is_RGBA<T>::value)
			{
				return GL_RGBA;
			}
			if constexpr (std::is_same<T, unsigned char>::value)
			{
				return GL_RED;
			}
			// TODO static assert
			assert(false, std::string("Unsuported texture format for OpenGL: ") + typeid(T).name());
		}

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
			glTexImage2D(GL_TEXTURE_2D, 0, GL_PIXEL_FORMAT(), m_img.width(), m_img.height(), 0, GL_PIXEL_FORMAT(), GL_PIXEL_TYPE(), m_img.data());
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