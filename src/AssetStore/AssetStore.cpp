#include "./AssetStore.h"
#include "../Logger/Logger.h"
#include <SDL2/SDL_image.h>

AssetStore::AssetStore()
{
	Logger::log("AssetStore constructor called!");
}

AssetStore::~AssetStore()
{
	clear_assets();
	Logger::log("AssetStore destructor called!");
}

void AssetStore::clear_assets()
{
	for (auto texture: textures)
	{
		SDL_DestroyTexture(texture.second);
	}
	textures.clear();
}

void AssetStore::add_texture(SDL_Renderer* renderer, const std::string& asset_id, const std::string& file_path)
{
	SDL_Surface* surface = IMG_Load(file_path.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	// Add the texture to the map
	textures.emplace(asset_id, texture);
	Logger::log("New texture added to the Asset Store with id = " + asset_id);
}

SDL_Texture* AssetStore::get_texture(const std::string& asset_id)
{
	return textures[asset_id];
}
