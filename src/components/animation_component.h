#ifndef ANIMATIONCOMPONENT_H
#define ANIMATIONCOMPONENT_H

#include <SDL2/SDL.h>

struct animation_component
{
	int num_frames;
	int current_frame;
	int frame_speed_rate;
	bool is_loop;
	int start_time;
};

#endif