#pragma once

#include <glm/glm.hpp>

template <int N, class T, class Out>
Out& operator<<(Out& out, glm::vec<N, T> const& vec)
{
	out << "[";
	for (int i = 0; i < N - 1; ++i)
	{
		out << vec[i] << ", ";
	}
	out << vec[N-1]<<"]";
	return out;
}

template <int R, int C, class T, class Out>
Out& operator<<(Out& out, glm::mat<C, R, T> const& mat)
{
	for (int j = 0; j < R ; ++j) // |
	{
		out << "[";
		for (int i = 0; i < C-1; ++i) // --
		{
			out << mat[i][j] << ", "; 
		}
		out << mat[C - 1][j] << "]";
		if (j < R - 1)
			out << '\n';
	}
	
	return out;
}