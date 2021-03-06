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

        void computeTangents()
        {
            // https://marti.works/posts/post-calculating-tangents-for-your-mesh/post/
            // https://learnopengl.com/Advanced-Lighting/Normal-Mapping
            uint number_of_triangles = m_indices.size() / 3;

            struct tan 
            {
                Vector3 tg, btg;
            };

            std::vector<tan> tans(m_vertices.size(), tan{ Vector3(0), Vector3(0) });

            for (uint triangle_index = 0; triangle_index < number_of_triangles; ++triangle_index)
            {
                uint i0 = m_indices[triangle_index*3];
                uint i1 = m_indices[triangle_index*3 +1];
                uint i2 = m_indices[triangle_index*3 +2];

                Vector3 pos0 = m_vertices[i0].position;
                Vector3 pos1 = m_vertices[i1].position;
                Vector3 pos2 = m_vertices[i2].position;

                Vector2 tex0 = m_vertices[i0].uv;
                Vector2 tex1 = m_vertices[i1].uv;
                Vector2 tex2 = m_vertices[i2].uv;

                Vector3 edge1 = pos1 - pos0;
                Vector3 edge2 = pos2 - pos0;

                Vector2 uv1 = tex1 - tex0;
                Vector2 uv2 = tex2 - tex0;

                // Implicit matrix [uv1[1], -uv1[0]; -uv2[1], uv2[0]]
                // The determinent of this matrix
                Float d = Float(1) / (uv1[0] * uv2[1] - uv1[1] * uv2[0]);

                Vector3 tg = Vector3{ 
                    ((edge1[0] * uv2[1]) - (edge2[0] * uv1[1])),
                    ((edge1[1] * uv2[1]) - (edge2[1] * uv1[1])),
                    ((edge1[2] * uv2[1]) - (edge2[2] * uv1[1])),
                } * d;

                Vector3 btg = Vector3{
                    ((edge1[0] * uv2[0]) - (edge2[0] * uv1[0])),
                    ((edge1[1] * uv2[0]) - (edge2[1] * uv1[0])),
                    ((edge1[2] * uv2[0]) - (edge2[2] * uv1[0])),
                } * d;

                tans[i0].tg += tg;
                tans[i1].tg += tg;
                tans[i2].tg += tg;

                tans[i0].btg += btg;
                tans[i1].btg += btg;
                tans[i2].btg += btg;
            }

            for (uint vertex_id = 0; vertex_id < m_vertices.size(); ++vertex_id)
            {
                Vector3 normal = m_vertices[vertex_id].normal;
                Vector3 tg = tans[vertex_id].tg;
                Vector3 btg = tans[vertex_id].btg;

                Vector3 t = tg - (normal * glm::dot(normal, tg));
                t = glm::normalize(t);

                Vector3 c = glm::cross(normal, tg);
                Float w = (glm::dot(c, btg) < 0) ? -1 : 1;
                m_vertices[vertex_id].tangent = t * w;
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
            res.computeTangents();
			return res;
		}

        static Shape Disk(uint N = 20, Vector3 const& center_pos = Vector3{ 0, 0, 0 }, Float radius = 1, bool up = true, Float shift=0)
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
                Float phi = (Float(i) + shift) / Float(N) * glm::two_pi<Float>();

                Vector2 circle_point = { std::cos(phi), std::sin(phi) };
                Vector3 pos = center.position + radius * Vector3{circle_point[0], 0, circle_point[1]};
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
            res.computeTangents();
            return res;
        }

        static Shape CylinderRing(Float r_up = 1.0, Float r_down = 1.0, uint N = 20, Vector3 center = { 0, 0, 0 }, Float h=0.5, bool shifted=false)
        {
            Shape res;
            res.reserve(2 * N+2, 2 * 3 * N);
            Float shift = shifted ? 0.5 : 0;
            for (int i = 0; i < N+1; ++i)
            {
                Float u_up = Float(i) / Float(N);
                Float u_down = (Float(i) + shift) / Float(N);
                Float phi_up = u_up * glm::two_pi<Float>();
                Float phi_down = u_down * glm::two_pi<Float>();

                Vector2 circle_point_up = { std::cos(phi_up), std::sin(phi_up) };
                Vector2 circle_point_down = { std::cos(phi_down), std::sin(phi_down) };

                Vertex up = { center + r_up * Vector3(circle_point_up[0], h, circle_point_up[1]), {circle_point_up[0], 0, circle_point_up[1]}, {u_up, 1} };
                Vertex down = { center + r_down * Vector3(circle_point_down[0], -h, circle_point_down[1]), {circle_point_down[0], 0, circle_point_down[1]}, {u_down, 0} };

                res.m_vertices.push_back(up);
                res.m_vertices.push_back(down);
                
                if (i < N)
                {
                    uint up_id = 2 * i;
                    uint down_id = 2 * i + 1;
                    uint next_up = (2 * i + 2);
                    uint next_down = (2 * i + 3);

                    res.addFace(up_id, next_up, down_id);
                    res.addFace(next_down, down_id, next_up);
                }
            }
            res.computeTangents();
            return res;
        }

        static Shape Cylinder(Float r_up = 1.0, Float r_down = 1.0, uint N = 20, bool up_disk=false, bool down_disk = false, bool shifted=false)
        {
            const Float h = 0.5;
            Vector3 center(0);
            Shape res = CylinderRing(r_up, r_down, N, center, h, shifted);

            if (up_disk)
            {
                Shape disk = Disk(N, center + Vector3{ 0, h, 0 }, r_up, true);
                res.merge(disk);
            }
            if (down_disk)
            {
                Float shift = shifted ? 0.5 : 0;
                Shape disk = Disk(N, center + Vector3{ 0, -h, 0 }, r_down, false, shift);
                res.merge(disk);
            }
            return res;
        }
	};
}