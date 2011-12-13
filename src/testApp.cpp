#include "testApp.h"

#define RECONNECT_TIME 400
#define MSG_START "MSGSTRT"
#define MSG_END   "MSGEND"
#define TCP_HOST_IP "127.0.0.1"

extern ofColor white;
extern ofColor black;
extern ofColor yellow;
extern ofColor gray;
extern ofColor blue;

//--------------------------------------------------------------
void testApp::setup(){
	// Network Setup
	
	ofSetVerticalSync(true);
	
	
	
	connectTime = 0;
	deltaTime = 0;
	
	
	//MIDI Setup
	
	int numProgs=loadProgramNames("midiPrograms.ini");
	report.loadFont("fonts/DinC.ttf");
	background.loadImage("images/background.jpg");
	kb.setup(ofGetWidth()-100., 4);
	for(int i=0; i<numProgs; i++)
		test.setValue(getProgramNames()[i]);
	testBut.setTitle("reset");
	testBut.setTextSize(24);
	
	rmp.setup();
	//rmp.registerEvents();
	
	keyboard.openPort("KeyRig 49");//"KeyRig 49"
	rolandSynth.openPort("decoder");

  title.loadFont("fonts/Din.ttf", 50);
}

long timeCL;

//--------------------------------------------------------------
void testApp::update(){
	rmp.update(75);
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground(0x33, 0x33, 0x33);
	
	
  
  ofSetColor(black);
  drawHatching(0,0,ofGetWidth(),ofGetHeight(), 25,1);
	
	rmp.draw(0, title.h);
	title.draw("PATCH INSTRUMENTS TO THE KEYBOARD", 0, 0);
  
  
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key==' '){
    vector<unsigned char> msg;
    msg.push_back(0x90);
    msg.push_back(51);
    msg.push_back(45);
    midiToSend(msg);
  }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	rmp.motion(x, y);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	rmp.drag(x, y);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	rmp.clickDown(x, y);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	rmp.clickUp(x, y);
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	rmp.resize(w, h);
}

void testApp::midiToSend(vector< unsigned char > message){
	
  rolandSynth.sendMessage(message);
	cout << "Message begin:";
  for (int i=0; i<message.size(); i++) {
    cout << int(message[i])<< ':';
  }
  cout << "Message End\n";
}

//


void testApp::midiReceived(double deltatime, std::vector< unsigned char > *message, int port){
  /*cout << "Message Begin:";
  for (unsigned int i=0; i<message->size(); i++) {
    cout << int(message->at(i)) << ":";
  }
  cout << "Message End\n";*/
	if(message->size()>=3){
		if ((message->at(0)==MIDI_NOTE_ON&&port==keyboard.getPort())||1) {
      //midiToSend(*message);
      
			if(message->at(1)>=MIDI_KEYBOARD_START&&message->at(1)<MIDI_KEYBOARD_END){
				rmp.kb[message->at(1)-MIDI_KEYBOARD_START].setPressed(message->at(2));
				if(!rmp.kb.getButtonChoice(message->at(1)-MIDI_KEYBOARD_START)){
					vector<unsigned char> msg;
					msg.push_back(MIDI_NOTE_ON+rmp.kb.getChannel());
					msg.push_back(message->at(1));
					msg.push_back(message->at(2));
					midiToSend(msg);
				}
				else {
					vector<instrument> & tmp=rmp.kb.getNotes(message->at(1)-MIDI_KEYBOARD_START);
					for (unsigned int i=0; i<tmp.size(); i++) {
						if(!tmp[i].isDefault()){
							tmp[i].play(message->at(2));
						}
					}
				}
			}
		}
	}
}