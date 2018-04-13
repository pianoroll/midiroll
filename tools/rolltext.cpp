//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri Apr 13 13:30:54 PDT 2018
// Last Modified: Fri Apr 13 13:30:57 PDT 2018
// Filename:      midiroll/tools/rolltext.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Read/write/change text meta messages in MIDI roll files.
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>
#include <string>

using namespace std;

// function declarations:
void processMidiFile(MidiRoll& rollfile, Options& options);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.process(argc, argv);
	MidiRoll midiroll;
	if (options.getArgCount() == 0) {
		midiroll.read(cin);
		processMidiFile(midiroll, options);
	} else {
		for (int i=0; i<options.getArgCount(); i++) {
			midiroll.read(options.getArg(i+1));
			processMidiFile(midiroll, options);
		}
	}
	return 0;
}


//////////////////////////////
//
// processMidiFile --
//

void processMidiFile(MidiRoll& rollfile, Options& options) {
	vector<MidiEvent*> list = rollfile.getTextMetaMessages();
	string text;
	for (int i=0; i<list.size(); i++) {
		int count = list[i]->getSize() - 2;
		text.clear();
		for (int j=0; j<count; j++) {
			text.push_back(list[i]->operator[](j+2));
		}
		cout << text << endl;
	}
}




