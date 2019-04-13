//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Apr  9 12:02:44 EDT 2019
// Last Modified: Tue Apr  9 12:02:46 EDT 2019
// Filename:      midiroll/tools/mirror.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Reverse the pitch order from high to low to low to high.
//                Add a mirroring pitch later, or a roll-specific control.
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>
#include <string>

using namespace std;
using namespace smf;

// function declarations:
void    processMidiFile    (MidiRoll& rollfile, Options& options);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.process(argc, argv);
	if ((options.getArgCount() > 2) || (options.getArgCount() == 1)) {
		cerr << "Usage: " << options.getCommand() << " input.mid output.mid" << endl;
		exit(1);
	}
	MidiRoll midiroll;
	if (options.getArgCount() == 0) {
		midiroll.read(cin);
		processMidiFile(midiroll, options);
		cout << midiroll << endl;
	} else {
		midiroll.read(options.getArg(1));
		processMidiFile(midiroll, options);
		midiroll.write(options.getArg(2));
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////

//////////////////////////////
//
// processMidiFile --
//

void processMidiFile(MidiRoll& rollfile, Options& options) {
	for (int i=0; i<rollfile.getTrackCount(); i++) {
		MidiEventList* mel = &rollfile[i];
		for (int j=0; j<mel->getEventCount(); j++) {
			MidiEvent* me = &(*mel)[j];
			if (me->isNote()) {
				int key = 127 - me->getKeyNumber();
				me->setKeyNumber(key);
			}
		}
	}
}



