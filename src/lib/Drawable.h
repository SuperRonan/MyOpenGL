#pragma once

#include <memory>

#include "Mesh.h"
#include "Material.h"

namespace lib
{

	template <class Float>
	struct Drawable : public Component
	{
	protected:

	public:

		std::shared_ptr<Mesh<Float>> mesh;
		std::shared_ptr<Material> material;

		Drawable(std::shared_ptr<Mesh<Float>> const& mesh, std::shared_ptr<Material> const& material) :
			mesh(mesh),
			material(material)
		{}

		virtual ~Drawable() = default;
	};

}