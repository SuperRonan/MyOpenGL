#pragma once

#include "Vertex.h"
#include <vector>
#include <string>
#include <glad/glad.h>

namespace lib
{
	template <class Float>
	class Mesh
	{
	protected:

		using Vertex = Vertex<Float>;
		
		GLuint m_VAO, m_VBO, m_EBO;

		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;



	public:

		Mesh() :
			m_VAO(0),
			m_VBO(0),
			m_EBO(0)
		{}

		void set(std::vector<Vertex> const& vertices, std::vector<GLuint> const& indices)
		{
			m_vertices = vertices;
			m_indices = indices;
		}

		void setup()
		{
			glGenVertexArrays(1, &m_VAO);
			glGenBuffers(1, &m_VBO);
			glGenBuffers(1, &m_EBO);

			glBindVertexArray(m_VAO);

			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
			glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vertex::stride(), (void*)Vertex::positionOffset());

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Vertex::stride(), (void*)Vertex::normalOffset());

			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, Vertex::stride(), (void*)Vertex::uvOffset());

			glBindVertexArray(0);
		}
		
		void draw()const
		{
			glBindVertexArray(m_VAO);

			glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);

			bindNone();
		}

		static void bindNone()
		{
			glBindVertexArray(0);
		}
	};
}