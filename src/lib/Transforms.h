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

	template <int N, class Float>
	glm::vec<N + 1, Float> homogenize(glm::vec<N, Float> const& vec, Float h=Float(1.0))
	{
		return glm::vec<N + 1, Float>(vec, h);
	}

	template <int N, class Float>
	glm::vec<N, Float> deHomogenize(glm::vec<N + 1, Float> const& h_vec)
	{
		glm::vec<N, Float> res;
		if (h_vec[N] != 0)
		{
			for (int i = 0; i < N; ++i)
				res[i] = h_vec[i] / h_vec[N];
		}
		else
		{
			for (int i = 0; i < N; ++i)
				res[i] = h_vec[i];
		}
		return res;
	}
}