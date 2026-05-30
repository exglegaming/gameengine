#ifndef ASSETSTORE_H
#define ASSETSTORE_H

#include <map>
#include <string>
#include <SDL2/SDL.h>

class AssetStore
{
	private:
		std::map<std::string, SDL_Texture*> textures;
		// TODO: Create map for fonts
		// TODO: Create map for audio

	public:
		AssetStore();
		~AssetStore();

		void clear_assets();
		void add_texture(SDL_Renderer* renderer, const std::string& asset_id, const std::string& file_path);
		SDL_Texture* get_texture(const std::string& asset_id);
};

#endif