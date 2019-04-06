//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat Apr  6 02:45:50 EDT 2019
// Last Modified: Sat Apr  6 02:45:52 EDT 2019
// Filename:      midiroll/tools/countnotes.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Count the number of notes for each MIDI key.  This program
//                can be used to analyze/verify the type of piano roll that
//                the MIDI roll represents.
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
	options.define("r|welte-red|red-welte=b", "Check if roll is likely red welte");
	options.define("B|bad|bad-only=b", "Only report if MIDI file is bad");
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


///////////////////////////////////////////////////////////////////////////

//////////////////////////////
//
// processMidiFile --
//

void processMidiFile(MidiRoll& rollfile, Options& options) {
	vector<int> keycount(128, 0);
	for (int i=0; i<rollfile.getTrackCount(); i++) {
		MidiEventList* mel = &rollfile[i];
		for (int j=0; j<mel->getEventCount(); j++) {
			MidiEvent* me = &(*mel)[j];
			if (me->isNoteOn()) {
				int key = me->getKeyNumber();
				keycount[key]++;
			}
		}
	}

	if (options.getBoolean("welte-red")) {
		int k104 = keycount[104];  // rewind hole: ideally one note only
		int k105 = keycount[105];  // this hole is not normally used, so expect 0.
		bool goodQ = true;
		if (k104 != 1) {
			goodQ = false;
		}
		if (k105 != 0) {
			goodQ = false;
		}
		if (!options.getBoolean("bad-only")) {
			if (goodQ) {
				cout << "OK\t" << rollfile.getFilename() << endl;
			} else {
				cout << "BAD\t" << rollfile.getFilename() << endl;
			}
		} else {
			if (!goodQ) {
				cout << rollfile.getFilename() << endl;
			}
		}
	} else {
		// List counts of MIDI files by default:
		for (int i=0; i<(int)keycount.size(); i++) {
			cout << i << ":\t" << keycount[i] << endl;
		}
	}
}



