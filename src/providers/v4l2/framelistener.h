#pragma once

#include <stdint.h>

enum pixelformat_t {
	RGB24,
	RGB32,
	YUYV
};

class FrameListener {

	private:

	public:
		FrameListener();

		virtual ~FrameListener();

		virtual void ProcessFrame(const uint8_t *buffer, int width, int height, pixelformat_t format);
};

