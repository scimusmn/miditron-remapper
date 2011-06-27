/*
 *  timeline.cpp
 *  Miditron
 *
 *  Created by Exhibits on 3/30/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "timeline.h"

midiConductor::midiConductor()
{
	cursorPos=0;
	speed=1;
}

void midiConductor::setup(double totalTime, double pixPerSecond)
{
	setPlaybackTime(totalTime);
	pixelsPerSecond(pixPerSecond);
}

void midiConductor::setPlaybackTime(double seconds)
{
	playTime=seconds;
	metronome.set(playTime);
	metronome.pause();
}

void midiConductor::setPlaybackSpeed(double times_normal)
{
	speed=times_normal;
}

void midiConductor::pixelsPerSecond(double numPixels)
{
	pps=numPixels;
}

void midiConductor::setCursorPosition(double pos)
{
	int totPix=playTime*pps;
	cursorPos=pos;
	metronome.setPercent(pos/totPix);
}

void midiConductor::update()
{
	if(bPlaying)
		cursorPos=speed*pps*(metronome.getElapsedf());
}

double midiConductor::getElapsed(){
	return metronome.getElapsedf();
}

bool midiConductor::isPlaying()
{
	return bPlaying;
}

void midiConductor::play()
{
	bPlaying=true;
	metronome.run();
}

void midiConductor::pause()
{
	bPlaying=false;
	metronome.pause();
}

void midiConductor::stop()
{
	metronome.reset();
}

void midiConductor::reset()
{
	cursorPos=0;
	metronome.setPercent(0);
}

double midiConductor::cursor()
{
	return cursorPos;
}