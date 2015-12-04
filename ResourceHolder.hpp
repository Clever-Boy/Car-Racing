#pragma once


#include <SFML/System/NonCopyable.hpp>

#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>


template <typename Resource, typename Identifier>
class ResourceHolder final : private sf::NonCopyable
{
	using ResourceMap = std::map<Identifier, std::unique_ptr<Resource>>;


public:
	void					load(Identifier id, const std::string& filename);

	Resource&				get(Identifier id);
	const Resource&			get(Identifier id) const;


private:
	void					insertResource(Identifier id, std::unique_ptr<Resource> resource);


private:
	ResourceMap				mResourceMap;
};

#include "ResourceHolder.inl"
