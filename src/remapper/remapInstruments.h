/*
 *  remapInstruments.h
 *  MidiRemapper
 *
 *  Created by Exhibits on 4/5/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#pragma once

#include "instrument.h"
#include "band.h"

class remapInst : public instrument{
protected:
public:
	remapInst():instrument(){}
	remapInst(string title, unsigned char channel,unsigned char note, bool repeat);
	void operator=(const instrument & t);
	void mouseMotion(double _x, double _y);
	void draw(double _x, double _y);
	bool clickDown(double _x, double _y);
	bool clickUp(vector<instrument> & compare);
	bool clickUp();
	void drawBackground();
	bool over(instrument & compare, int i);
};

class remapBand : public bandBar{
protected:
	vector<remapInst> rInstruments;
public:
	remapBand():bandBar(){}
	void setHeight(double hgt,double top=0, double bot=0);
	void setup(xmlParse * config, double hgt=0);
	void draw(double _x, double _y);
	void drawDraggedBlocks();
	bool clickUp(vector<instrument> & comp);
	bool clickUp();
	bool clickDown(int _x, int _y);
	bool mouseMotion(int _x, int _y);
	instrument & lastInstrument()
	{
		return rInstruments[lastInst];
	}
};