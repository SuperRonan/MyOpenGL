#pragma once

#include "Math.h"
#include <glm/gtc/matrix_transform.hpp>

namespace lib
{
	template <int N, class Float>
	MatrixN<N, Float> scaleMatrix(Vector<N-1, Float> const& vector)
	{
		MatrixN<N, Float> res(Float(1.0));
		for (int i = 0; i < N-1; ++i)
		{
			res[i][i] = vector[i];
		}
		return res;
	}

	template <int N, class Float>
	MatrixN<N, Float> scaleMatrix(Float s)
	{
		MatrixN<N, Float> res(Float(1.0));
		for (int i = 0; i < N - 1; ++i)
		{
			res[i][i] = s;
		}
		return res;
	}

	template <int N, class Float>
	MatrixN<N, Float> translateMatrix(Vector<N - 1, Float> const& vector)
	{
		MatrixN<N, Float> res(Float(1.0));
		for (int i = 0; i < N - 1; ++i)
		{
			res[N-1][i] = vector[i];
		}
		return res;
	}

	template <int N, class Float>
	MatrixN<N, Float> inverseTranslateMatrix(Vector<N - 1, Float> const& vector)
	{
		MatrixN<N, Float> res(Float(1.0));
		for (int i = 0; i < N - 1; ++i)
		{
			res[N - 1][i] = -vector[i];
		}
		return res;
	}

	template <int N, class Float>
	MatrixN<N, Float> inverseTranslateMatrix(MatrixN<N, Float> const& t_mat)
	{
		MatrixN<N, Float> res(Float(1.0));
		for (int i = 0; i < N - 1; ++i)
		{
			res[N - 1][i] = -t_mat[N-1][i];
		}
		return res;
	}

	template <int N, class Float>
	Vector<N + 1, Float> homogenize(Vector<N, Float> const& vec, Float h=Float(1.0))
	{
		return Vector<N + 1, Float>(vec, h);
	}

	template <int N, class Float>
	glm::vec<N, Float> deHomogenize(Vector<N + 1, Float> const& h_vec)
	{
		Vector<N, Float> res;
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