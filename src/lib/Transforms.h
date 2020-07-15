#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace lib
{
	template <int N, class Float>
	glm::mat<N, N, Float> scaleMatrix(glm::vec<N-1, Float> const& vector)
	{
		glm::mat<N, N, Float> res(Float(1.0));
		for (int i = 0; i < N-1; ++i)
		{
			res[i][i] = vector[i];
		}
		return res;
	}

	template <int N, class Float>
	glm::mat<N, N, Float> translateMatrix(glm::vec<N - 1, Float> const& vector)
	{
		glm::mat<N, N, Float> res(Float(1.0));
		for (int i = 0; i < N - 1; ++i)
		{
			res[N-1][i] = vector[i];
		}
		return res;
	}

	template <int N, class Float>
	glm::mat<N, N, Float> inverseTranslateMatrix(glm::vec<N - 1, Float> const& vector)
	{
		glm::mat<N, N, Float> res(Float(1.0));
		for (int i = 0; i < N - 1; ++i)
		{
			res[N - 1][i] = -vector[i];
		}
		return res;
	}

	template <int N, class Float>
	glm::mat<N, N, Float> inverseTranslateMatrix(glm::mat<N, N, Float> const& t_mat)
	{
		glm::mat<N, N, Float> res(Float(1.0));
		for (int i = 0; i < N - 1; ++i)
		{
			res[N - 1][i] = -t_mat[N-1][i];
		}
		return res;
	}
}