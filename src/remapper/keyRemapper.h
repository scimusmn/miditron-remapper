/*
 *  keyRemapper.h
 *  MidiRemapper
 *
 *  Created by Exhibits on 4/5/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#pragma once

#include "mapKeyboard.h"
#include "remapInstruments.h"

class remapper: public ofInterObj{
protected:
	ofButton reset;
	double xDis, yDis;
  ofImage controlBackground;
	ofImage pianoBackground;
	remapInst held;
	ofFont dinc;
public:
	remapKeyboard kb;
	remapBand band;
	remapper():ofInterObj(){}
	void setup();
	void update();
	void draw(double _x, double _y);
	bool clickDown(int _x, int _y);
	bool clickUp(int _x, int _y);
	void drag(int _x, int _y);
	void resize(int _x, int _h);
};
