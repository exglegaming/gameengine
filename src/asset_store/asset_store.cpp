#include "./asset_store.h"
#include "../logger/logger.h"
#include <SDL2/SDL_image.h>

asset_store::asset_store()
{
	logger::log("AssetStore constructor called!");
}

asset_store::~asset_store()
{
	clear_assets();
	logger::log("AssetStore destructor called!");
}

void asset_store::clear_assets()
{
	for (auto texture: textures)
	{
		SDL_DestroyTexture(texture.second);
	}
	textures.clear();
}

void asset_store::add_texture(SDL_Renderer* renderer, const std::string& asset_id, const std::string& file_path)
{
	SDL_Surface* surface = IMG_Load(file_path.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	// Add the texture to the map
	textures.emplace(asset_id, texture);
	logger::log("New texture added to the Asset Store with id = " + asset_id);
}

SDL_Texture* asset_store::get_texture(const std::string& asset_id)
{
	return textures[asset_id];
}
