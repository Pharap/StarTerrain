//
//  Copyright (C) 2019 Pharap (@Pharap)
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include <Arduboy2.h>

Arduboy2 arduboy;

#include "Stars.h"

struct Camera
{
	static constexpr uint8_t width = WIDTH;
	static constexpr uint8_t height = HEIGHT;

	Point position;
	
	constexpr uint8_t getWidth() const
	{
		return width;
	}
	
	constexpr uint8_t getHeight() const
	{
		return height;
	}
	
	constexpr int16_t getLeft() const
	{
		return this->position.x;
	}
	
	constexpr int16_t getTop() const
	{
		return this->position.y;
	}
	
	constexpr int16_t getRight() const
	{
		return (this->position.x + width);
	}
	
	constexpr int16_t getBottom() const
	{
		return (this->position.y + height);
	}
};

Camera camera;

// This function is the state function of an
// xorshift pseudo-random number generator
// and the values are taken from Wikipedia
// https://en.wikipedia.org/wiki/Xorshift#Example_implementation
// which are in turn from a document
// written by George Marsaglia.
uint32_t hash(uint32_t value)
{
	value ^= (value << 13);
	value ^= (value >> 17);
	value ^= (value << 5);
	return value;
}

uint32_t hash(uint16_t x, uint16_t y)
{
	const uint32_t x2 = x;
	const uint32_t y2 = y;
	const uint32_t value = ((x2 << 16) | (y2 << 0));

	return hash(value * (x2 ^ y2));
}

uint32_t hash(int16_t x, int16_t y)
{
	return hash(static_cast<uint16_t>(x), static_cast<uint16_t>(y));
}

template< size_t size > constexpr size_t getFrameCount(const unsigned char (&)[size], size_t frameWidth, size_t frameHeight)
{
	return ((size - (sizeof(unsigned char) * 2)) / ((frameWidth * frameHeight) / 8));
}

uint8_t generateStarIndex(int16_t x, int16_t y)
{
	// Hash the coordinates, then map to a sprite index
	return static_cast<uint8_t>(hash(x, y) % getFrameCount(starSprites, starWidth, starHeight));
}

void renderStars()
{
	// Calculate the coordinates that are actually on screen
	// The extra -1 is needed because integer division truncates instead of flooring.
	const int16_t startX = (camera.getLeft() >= 0) ? (camera.getLeft() / starWidth) : ((camera.getLeft() / starWidth) - 1);
	const int16_t startY = (camera.getTop() >= 0) ? (camera.getTop() / starHeight) : ((camera.getTop() / starHeight) - 1);
	
	const int16_t endX = (camera.getRight() / starWidth);
	const int16_t endY = (camera.getBottom() / starHeight);

	for(int16_t y = startY; y <= endY; ++y)
		for(int16_t x = startX; x <= endX; ++x)
		{
			const int16_t drawX = ((x * starWidth) - camera.position.x);
			const int16_t drawY = ((y * starHeight) - camera.position.y);			
			const uint8_t starIndex = generateStarIndex(x, y);
			
			Sprites::drawOverwrite(drawX, drawY, starSprites, starIndex);
		}
}

void setup()
{
	arduboy.begin();
}

void loop()
{
	if(!arduboy.nextFrame())
		return;
		
	arduboy.pollButtons();
	
	if(arduboy.pressed(LEFT_BUTTON))
		--camera.position.x;
	
	if(arduboy.pressed(RIGHT_BUTTON))
		++camera.position.x;
	
	if(arduboy.pressed(UP_BUTTON))
		--camera.position.y;
	
	if(arduboy.pressed(DOWN_BUTTON))
		++camera.position.y;
	
	arduboy.clear();
	
	renderStars();
	
	arduboy.display();
}
