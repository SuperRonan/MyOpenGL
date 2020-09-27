#pragma once

#include "Vertex.h"
#include <vector>
#include <string>
#include <glad/glad.h>
#include <type_traits>
#include "Shapes.h"

namespace lib
{
	template <class Float>
	class Mesh
	{
	protected:

		using Vertex = Vertex<Float>;

		using Shape = Shape<Float, GLuint>;
		
		GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;

		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;

		GLuint m_number_of_elements = 0;

		bool m_host_is_loaded = false;

		static constexpr GLenum GL_Float()
		{
			if constexpr (std::is_same<Float, float>::value)
				return GL_FLOAT;
			else
				return GL_DOUBLE;
		}

	public:

		Mesh(){}

		Mesh(Mesh&& other) = default;

		Mesh(Mesh const& other) = delete;

		Mesh(Shape const& shape)
		{
			set(shape);
		}

		Mesh(Shape && shape)
		{
			set(std::move(shape));
		}

		~Mesh()
		{
			if (m_VAO)
			{
				glDeleteVertexArrays(1, &m_VAO);
				glDeleteBuffers(2, &m_VBO); // and also EBO
			}
		}

		void set(Shape const& shape)
		{
			set(shape.m_vertices, shape.m_indices);
		}

		void set(Shape && shape)
		{
			set(std::move(shape.m_vertices), std::move(shape.m_indices));
		}


		void set(std::vector<Vertex> const& vertices, std::vector<GLuint> const& indices)
		{
			m_vertices = vertices;
			m_indices = indices;
			m_number_of_elements = indices.size();
			m_host_is_loaded = true;
		}

		void set(std::vector<Vertex>&& vertices, std::vector<GLuint>&& indices)
		{
			m_vertices = std::move(vertices);
			m_indices = std::move(indices);
			m_number_of_elements = m_indices.size();
			m_host_is_loaded = true;
		}

		void clearHost()
		{
			m_vertices.clear();
			m_vertices.shrink_to_fit();
			m_indices.clear();
			m_indices.shrink_to_fit();
			m_host_is_loaded = false;
		}

		void setup()
		{
			assert(m_host_is_loaded);

			glGenVertexArrays(1, &m_VAO);
			glGenBuffers(2, &m_VBO); // and also EBO

			glBindVertexArray(m_VAO);

			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
			glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_Float(), GL_FALSE, Vertex::stride(), (void*)Vertex::positionOffset());

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_Float(), GL_FALSE, Vertex::stride(), (void*)Vertex::normalOffset());

			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, Vertex::stride(), (void*)Vertex::tangentOffset());

			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 2, GL_Float(), GL_FALSE, Vertex::stride(), (void*)Vertex::uvOffset());

			glBindVertexArray(0);
		}
		
		void draw()const
		{
			glBindVertexArray(m_VAO);

			glDrawElements(GL_TRIANGLES, m_number_of_elements, GL_UNSIGNED_INT, 0);

			bindNone();
		}

		static void bindNone()
		{
			glBindVertexArray(0);
		}
	};
}