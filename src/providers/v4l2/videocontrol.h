#pragma once

#include "jmedia/jvideodevicecontrol.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

namespace jmedia {

struct video_query_control_t {
	jvideo_control_t id;
	int v4l_id;
	int value;
	int default_value;
	int step;
	int minimum;
	int maximum;
};

class VideoControl {

	private:
		std::vector<video_query_control_t> _query_controls;
		int _handler;

	private:
		void EnumerateControls();

	public:
		VideoControl(int handler);

		virtual ~VideoControl();

		virtual double GetFramesPerSecond();

		virtual std::vector<jvideo_control_t> GetControls();
		
		virtual bool HasControl(jvideo_control_t id);

		virtual int GetValue(jvideo_control_t id);

		virtual bool SetValue(jvideo_control_t id, int value);

		virtual void Reset(jvideo_control_t id);

		virtual void Reset();
		
};

}

