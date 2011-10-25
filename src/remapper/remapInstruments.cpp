/*
 *  remapInstruments.cpp
 *  MidiRemapper
 *
 *  Created by Exhibits on 4/5/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "remapInstruments.h"

/*bool remapInst::clickUp(vector<instrument> & compare)
{
	base.clickUp();
	bool ret=false;
	for (unsigned int i=0; i<blocks.size(); i++) {
		if(blocks[i].clickUp()){
			lastBlock=i;
			for (unsigned int j=0; j<compare.size(); j++) {
				if (over(compare[j],i)) {
					compare[j]=*(this);
					compare[j].setDefault(false);
					ret=true;
				}
			}
			blocks.erase(blocks.begin()+i);
			lastBlock=-1;
			stop();
		}
	}
	bHolding=false;
	return ret;
}*/

bool remapInst::clickUp()
{
	base.clickUp();
	bool ret=false;
	for (unsigned int i=0; i<blocks.size(); i++) {
		if(blocks[i].clickUp()){
			ret=true;
			lastBlock=i;
			if (blocks[i].x<x+w) {
				ret=false;
				blocks.erase(blocks.begin()+i);
				lastBlock=-1;
			}
			stop();
		}
	}
	bHolding=false;
	return ret;
}

remapInst::remapInst(string title, unsigned char channel,unsigned char note)
{
	point=24;
  setup(title,channel,note);
	base.header.setSize(point);
	base.w=max(float(150), base.header.stringWidth(title)+20);
	base.h=max(float(20), base.header.stringHeight("Kjg")+10);
	w=base.w;
	h=base.h;
}

void remapInst::operator=(const instrument & t)
{
	w=t.w;
	base=t.base;
  base.color=t.base.color;
	bHolding=t.bHolding;
	blocks=t.blocks;
	title=t.title;
  setup(title,base.channel,base.note);
	scroll.x=t.scroll.x,scroll.y=t.scroll.y;
	lastBlock=t.lastBlock;
	bDefault=t.bDefault;
	point=t.point;
}


/*bool remapInst::over(instrument & t, int i)
{
	dragBlock & tmp=blocks[i];
	return ((tmp.x+tmp.xDisp>t.x&&tmp.x+tmp.xDisp<t.x+t.w)||(tmp.x+tmp.xDisp+tmp.w>=t.x&&tmp.x+tmp.xDisp+tmp.w<=t.x+t.w))&&\
	((tmp.y+tmp.yDisp>t.y&&tmp.y+tmp.yDisp<t.y+t.h)||(tmp.y+tmp.yDisp+tmp.h>=t.y&&tmp.y+tmp.yDisp+tmp.h<=t.y+t.h));
}*/

bool remapInst::clickDown(double _x, double _y)
{
	bool ret=0;
	if(!bHolding&&base.clickDown(_x, _y)){
		blocks.push_back(dragBlock(base));
		base.clickUp();
		relMouse.x=_x-base.x;
		relPos.y=_y-base.y;
		play();
		bHolding=true;
		ret=1;
	}
	else for (unsigned int i=0; i<blocks.size(); i++) {
		if(!bHolding&&blocks[i].clickDown(_x,_y)){
			play();
			bHolding=true;
			relMouse.x=_x-blocks[i].x-scroll.x;
			relPos.y=_y-blocks[i].y-scroll.y;
		}
	}
	return ret;
}

void remapInst::draw(double _x, double _y)
{
	x=_x, y=_y;
	base.draw(x,y);
}

void remapInst::mouseMotion(double _x, double _y)
{
	for (unsigned int i=0; i<blocks.size(); i++) {
		blocks[i].mouseMotion((blocks[i].pressed())?_x-relMouse.x:_x,_y);
	}
}

void remapInst::drawBackground()
{
	for (unsigned int i=0; i<blocks.size(); i++) {
		blocks[i].soundBlock::draw();
	}
}

void remapBand::setup(xmlParse * config,double hgt)
{
	clearBut.setTitle("clear blocks");
	clearBut.setTextSize(24);
	clearBut.setAvailable(true);
	sideBarBack.loadImage("images/background.jpg");
	string font=config->prop;
	for (unsigned int i=0; i<config->size(); i++) {
		xmlParse xml=config->child[i];
		if (xml.label=="instrument") {
			string col=xml.prop;
			string title=xml.name;
			long color=strtol(col.c_str(),NULL,0);
			int curInst=rInstruments.size();
			unsigned char note, channel;
			bool percussive=false;
			double delay=0;
			map<string,int> list;
			list["note"]=0;
			list["channel"]=1;
			list["percussive"]=2;
			list["delay"]=3;
			list["dropdown"]=4;
			for(int j=0;j<xml.size();j++){
				string * node=xml.getSubnode(j);
				switch (list.find(node[0])->second) {
					case 0:
						note=ofToInt(node[1]);
						break;
					case 1:
						channel=ofToInt(node[1]);
						break;
					case 2:
						percussive=true;
						break;
					case 3:
						delay=ofToFloat(node[1]);
						break;
					case 8:
						//ddGroup.push_back(ofDropDown(&xml->child[i]));
						break;
					default:
						break;
				}
			}
			rInstruments.push_back( remapInst(title,channel,note));
			rInstruments[curInst].setColor(color);
		}
	}
	setHeight(hgt);
}

void remapBand::draw(double _x, double _y)
{
	x=_x,y=_y;
	
	//sidebar background
	ofSetColor(255, 255, 255);
	double sideBarScale=(ofGetWidth()/sideBarBack.width);
	sideBarBack.draw(x, y+h-sideBarScale*sideBarBack.height,ofGetWidth(),sideBarScale*sideBarBack.height);
	
	//
	for(int i=0; i<numBins; i++){
		ofSetColor(0x999999);
		double binLeft=x+bar.w+(bar.w+binWidth)*i;
		double binRight=x+bar.w+binWidth+(bar.w+binWidth)*i;
    
    // shading behind the blocks in the bins
		ofShadowRounded(binLeft, y+yoff, binWidth, numBlocksPerBin*(binHeight+yBlockSpace), 15, 20);
		ofRect(binLeft, y+yoff, binWidth, numBlocksPerBin*(binHeight+yBlockSpace));
		
    ofSetShadowDarkness(.3);
		ofShade(binLeft, y+yoff, 10, h+yBlockSpace-yGap, OF_RIGHT);
		ofShade(binRight, y+yoff, 10, h+yBlockSpace-yGap, OF_LEFT);
		
    ofSetShadowDarkness(.2);
		ofShade(binLeft, y+h-yGap+yBlockSpace, 10, binWidth, OF_UP);
		ofShade(binLeft, y+yoff, 10, binWidth, OF_DOWN);
	}
	
	for (unsigned int i=0; i<rInstruments.size()&&i<16; i++) {
		rInstruments[i].draw(x+bar.w+(bar.w+binWidth)*(i/numBlocksPerBin)+xBlockSpace,y+yoff+yBlockSpace+(binHeight+yBlockSpace)*(i%numBlocksPerBin));
		if(i%numBlocksPerBin<numBlocksPerBin-1){
			double tmpY=rInstruments[i].y+binHeight-yBlockSpace;
      ofSetShadowDarkness(.3);
			ofShade(x+bar.w+(bar.w+binWidth)*(i/numBlocksPerBin), tmpY, 3, binWidth, OF_UP);
			ofShade(x+bar.w+(bar.w+binWidth)*(i/numBlocksPerBin), tmpY, 3, binWidth, OF_DOWN,false);
		}
	}
	
	//------ shading on the bottom of the bar
  ofSetShadowDarkness(.5);
	ofShade(x, y+yoff+h, 15, ofGetWidth(), OF_UP);
	//ofShade(x, y+yoff+h, 15, ofGetWidth(), OF_DOWN, .5);
	
	
	//bar.draw(x,_y);
	
	//Shades over ends of the scroll bar
  ofSetShadowDarkness(.3);
	ofShade(x, y+yoff, 5, bar.w+4, OF_DOWN);
	ofShade(x, y+yoff+viewSize, 5, bar.w+4, OF_UP);
	
	//clearBut.draw((w-clearBut.w)/2, getBottomPos()+((yBot)-clearBut.h)/2);
}

void remapBand::drawDraggedBlocks()
{
	for (unsigned int i=0; i<rInstruments.size(); i++) {
		rInstruments[i].drawBackground();
	}
}

bool remapBand::clickDown(int _x, int _y)
{
	bool ret=false;
	for (unsigned int i=0; i<rInstruments.size(); i++) {
		if(!bHolding&&_y>y+yoff&&_y<=y+yoff+viewSize&&rInstruments[i].over(_x,_y)){
			ret=bHolding=true;
			lastInst=i;
		}
	}
	bar.clickDown(_x, _y);
	return ret;
}

void remapBand::setHeight(double hgt,double top, double bot)
{
	h=(hgt)?hgt:ofGetHeight();
	yoff=top;
	yBot=bot;
	viewSize=h-(yoff+yBot);
	yGap=bar.w=20;
	xBlockSpace=10;
	yBlockSpace=5;
	numBlocksPerBin=4;
	numBins=min(16,int(rInstruments.size()));
  numBins=(numBins%4)?numBins/numBlocksPerBin+1:numBins/numBlocksPerBin;
	h=numBlocksPerBin*(binHeight+yBlockSpace)-yBlockSpace+yGap;
	for (unsigned int i=0; i<rInstruments.size(); i++) {
		binWidth=max(binWidth,rInstruments[i].w+xBlockSpace*2);
		binHeight=max(binHeight,rInstruments[i].h+yBlockSpace*2);
	}
	w=(bar.w+binWidth)*numBins+bar.w;
	double fullSize=binHeight*rInstruments.size();
	//bar.setup(20, viewSize, OF_VERT);
	//bar.registerArea(viewSize,fullSize);
}

/*bool remapBand::clickUp(vector<instrument> & comp)
{
	bool ret=false;
	for (unsigned int i=0; i<rInstruments.size(); i++) {
		if(rInstruments[i].clickUp(comp)){
			ret=true;
			lastInst=i;
		}
	}
	bHolding=false;
	clearBut.clickUp();
	bar.clickUp();
	return ret;
}*/

bool remapBand::clickUp()
{
	bool ret=false;
	for (unsigned int i=0; i<rInstruments.size(); i++) {
		if(rInstruments[i].clickUp()){
			ret=true;
			lastInst=i;
		}
	}
	bHolding=false;
	clearBut.clickUp();
	bar.clickUp();
	return ret;
}

bool remapBand::mouseMotion(int _x, int _y)
{
	for (unsigned int i=0; i<rInstruments.size(); i++) {
		rInstruments[i].mouseMotion(_x,_y);
	}
	if(bar.mouseMotion(_x,_y)){
		for (unsigned int i=0; i<rInstruments.size(); i++) {
			rInstruments[i].update(-bar.getScrollPosition(),OF_VERT);
		}
	}
}