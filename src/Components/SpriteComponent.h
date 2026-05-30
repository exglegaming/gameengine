#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <string>

struct SpriteComponent 
{
	std::string asset_id;
	int width;
	int height;

	SpriteComponent(std::string asset_id = "", int width = 0, int height = 0)
	{
		this->asset_id = asset_id;
		this->width = width;
		this->height = height;
	}
};

#endif