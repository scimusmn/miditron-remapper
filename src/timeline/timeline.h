/*
 *  timeline.h
 *  Miditron
 *
 *  Created by Exhibits on 3/30/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#pragma once

#include "ofExtended.h"

class midiConductor {
	ofTimer metronome;
	double playTime;
	double pps;
	double speed;
	double cursorPos;
	bool bPlaying;
public:
	midiConductor();
	void setup(double totalTime, double pixelsPerSecond);
	void setPlaybackTime(double seconds);
	void setPlaybackSpeed(double times_normal);
	void pixelsPerSecond(double numPixels);
	void setCursorPosition(double pos);
	double getElapsed();
	bool isPlaying();
	void update();
	void play();
	void reset();
	void pause();
	void stop();
	void rewind();
	double cursor();
	friend class midiSequencer;
};