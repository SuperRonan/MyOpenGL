#pragma once

#include "Math.h"
#include "Vertex.h"
#include <vector>


namespace lib
{
	template <class Float, class uint = unsigned int>
	class Shape
	{
	public:

		using Vertex = Vertex<Float>;
        using Vector3 = Vector3<Float>;
        using Vector2 = Vector2<Float>;

		std::vector<Vertex> m_vertices;
		std::vector<uint> m_indices;

		Shape() = default;
		Shape(Shape const&) = default;
		Shape(Shape &&) = default;

        void addFace(uint a, uint b, uint c)
        {
            m_indices.push_back(a);
            m_indices.push_back(b);
            m_indices.push_back(c);
        }

        void reserve(int n)
        {
            m_indices.reserve(n);
            m_vertices.reserve(n);
        }

        void reserve(int nv, int ni)
        {
            m_indices.reserve(nv);
            m_vertices.reserve(ni);
        }

        void merge(Shape const& other)
        {
            uint offset = m_vertices.size();
            m_vertices.insert(m_vertices.end(), other.m_vertices.begin(), other.m_vertices.end());
            m_indices.reserve(m_indices.size() + other.m_indices.size());
            for (int i = 0; i < other.m_indices.size(); ++i)
            {
                m_indices.push_back(other.m_indices[i] + offset);
            }
        }

		static Shape Cube(bool vertex_normals=false)
		{
			Shape res;

            //        (0,0)                      (h,0)    
            //          x0------------------------x1
            //         /|                        /|
            //        / |                       / |
            //       /  |                      /  |
            //      /   |                     /   |
            //(0,h)x3------------------------x2   | (h,h)
            //     |    |        +y          |    |
            //     |    |        ^           |    |
            //     |    |        |           |    |
            //     |    |        |           |    |
            //     |    | (h,0)  0--->+x     |    |
            //     |    x4------/------------|----x5 (1,h)  
            //     |   /       /             |   / 
            //     |  /       +z             |  /  
            //     | /                       | /   
            //     |/                        |/    
            //(h,h)x7------------------------x6 (1,1)

            const Float h = Float(0.5);
            const Float t = Float(1) / Float(3);
            const Float tt = Float(2) / Float(3);


            if (vertex_normals)
            {
                const auto makeVertex = [](Float x, Float y, Float z, Float u, Float v)
                {return lib::Vertex<Float>({ x, y, z }, { x, y, z }, { u, v }); };

                // half
                res.m_vertices= {
                    makeVertex(-h, h, -h, 0, 0), // x0
                    makeVertex(h, h, -h, h, 0), // x1
                    makeVertex(h, h, h, h, h), // x2
                    makeVertex(-h, h, h, 0, h), // x3
                    makeVertex(-h, -h, -h, h, 0), // x4
                    makeVertex(h, -h, -h, 1, h), // x5
                    makeVertex(h, -h, h, 1, 1), // x6
                    makeVertex(-h, -h, h, h, h), // x7
                };

                const auto addFace = [&res](uint v0, uint v1, uint v2, uint v3)
                {
                    res.m_indices.push_back(v0);
                    res.m_indices.push_back(v1);
                    res.m_indices.push_back(v2);
                    
                    res.m_indices.push_back(v2);
                    res.m_indices.push_back(v3);
                    res.m_indices.push_back(v0);
                };

                addFace(0, 3, 2, 1); // up (+y)
                addFace(0, 1, 5, 4); // back (-z)
                addFace(0, 4, 7, 3); // left (-x)
                addFace(6, 7, 4, 5); // bottom (-y)
                addFace(6, 2, 3, 7); // front (+z)
                addFace(6, 5, 1, 2); // right (+x)
            }
            else
            {
                lib::Vector3<Float> X(1, 0, 0), Y(0, 1, 0), Z(0, 0, 1);
                const auto sign = [](Float f) {return f > 0 ? Float(1) : (f < 0 ? Float(-1) : Float(0)); };
                const auto addVertexes = [&](Float x, Float y, Float z, Float u, Float v)
                {
                    res.m_vertices.emplace_back(lib::Vector3<Float>{x, y, z}, sign(x)* X, lib::Vector2<Float>{u, v});
                    res.m_vertices.emplace_back(lib::Vector3<Float>{x, y, z}, sign(y)* Y, lib::Vector2<Float>{u, v});
                    res.m_vertices.emplace_back(lib::Vector3<Float>{x, y, z}, sign(z)* Z, lib::Vector2<Float>{u, v});
                };

                addVertexes(-h, h, -h, 0, 0); // x0
                addVertexes(h, h, -h, h, 0); // x1
                addVertexes(h, h, h, h, h); // x2
                addVertexes(-h, h, h, 0, h); // x3
                addVertexes(-h, -h, -h, h, 0); // x4
                addVertexes(h, -h, -h, 1, h); // x5
                addVertexes(h, -h, h, 1, 1); // x6
                addVertexes(-h, -h, h, h, h); // x7

                const auto id = [](uint xi, uint axis) {return xi * 3 + axis; };

                const auto addFace = [&](uint v0, uint v1, uint v2, uint v3, uint axis)
                {
                    res.m_indices.push_back(id(v0, axis));
                    res.m_indices.push_back(id(v1, axis));
                    res.m_indices.push_back(id(v2, axis));
                    res.m_indices.push_back(id(v2, axis));
                    res.m_indices.push_back(id(v3, axis));
                    res.m_indices.push_back(id(v0, axis));
                };
                addFace(0, 3, 2, 1, 1); // up (+y)
                addFace(0, 1, 5, 4, 2); // back (-z)
                addFace(0, 4, 7, 3, 0); // left (-x)
                addFace(6, 7, 4, 5, 1); // bottom (-y)
                addFace(6, 2, 3, 7, 2); // front (+z)
                addFace(6, 5, 1, 2, 0); // right (+x)
            }

			return res;
		}

        static Shape Disk(uint N = 20, Vector3 const& center_pos = Vector3{ 0, 0, 0 }, Float radius = 1, bool up = true)
        {
            assert(N > 1);
            Shape res;
            res.reserve(N + 1, 3 * N);
            Vector3 normal = { 0, up ? 1 : -1, 0 };
            Vertex center = { center_pos, normal, {0.5, 0.5} };
            
            uint center_id = N;
            
            const Float h = 0.5;
            for (int i = 0; i < N; ++i)
            {
                Float phi = Float(i) / Float(N) * glm::two_pi<Float>();

                Vector2 circle_point = { std::cos(phi), std::sin(phi) };
                Vector3 pos = center.m_position + radius * Vector3{circle_point[0], 0, circle_point[1]};
                Vector2 uv = h * circle_point  + Vector2{0.5, 0.5};
                Vertex vertex = { pos, normal, uv };
                res.m_vertices.push_back(vertex);

                uint next = (i + 1) % N;
                if(up)
                    res.addFace(center_id, next, i);
                else
                    res.addFace(center_id, i, next);
            }
            res.m_vertices.push_back(center);
            return res;
        }

        static Shape Cylinder(Float r_up = 1.0, Float r_down = 1.0, uint N = 20, bool up_disk=false, bool down_disk = false)
        {
            Shape res;
            res.reserve(2 * N, 2 * 3 * N);
            const Float h = 0.5;
            Vector3 center(0);
            for (int i = 0; i < N; ++i)
            {
                Float u = Float(i) / Float(N);
                Float phi = u * glm::two_pi<Float>();
                Vector2 circle_point = { std::cos(phi), std::sin(phi) };

                Vertex up = { center + r_up * Vector3(circle_point[0], h, circle_point[1]), {circle_point[0], 0, circle_point[1]}, {u, 1} };
                Vertex down = { center + r_down * Vector3(circle_point[0], -h, circle_point[1]), {circle_point[0], 0, circle_point[1]}, {u, 0} };
            
                res.m_vertices.push_back(up);
                res.m_vertices.push_back(down);

                uint up_id = 2 * i;
                uint down_id = 2 * i + 1;
                uint next_up = (2 * i + 2) % (2 * N);
                uint next_down = (2 * i + 3) % (2 * N);

                res.addFace(up_id, next_up, down_id);
                res.addFace(next_down, down_id, next_up);
            }

            if (up_disk)
            {
                Shape disk = Disk(N, center + Vector3{ 0, h, 0 }, r_up, true);
                res.merge(disk);
            }
            if (down_disk)
            {
                Shape disk = Disk(N, center + Vector3{ 0, -h, 0 }, r_down, false);
                res.merge(disk);
            }

            return res;
        }
	};
}