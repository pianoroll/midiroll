//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sun Apr 21 15:42:44 EDT 2019
// Last Modified: Sun Apr 21 15:42:50 EDT 2019
// Filename:      midiroll/tools/roll2sv.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Convert a performance MIDI roll into SV annotations.
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>
#include <string>

using namespace std;
using namespace smf;

// function declarations:
void    processMidiFile    (MidiRoll& rollfile, Options& options);
void    extractNoteVolumes (MidiRoll& rollfile, int track);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("p|ped|pedal|sustain-pedal=b", "Extract sustain pedal");
	options.define("s|u|uc|soft|soft-pedal|una-corda=b", "Extract soft pedal");
	options.define("b|bass|bass-volume=b", "Extract bass dynamics");
	options.define("t|treble|treble-volume=b", "Extract treble dynamics");
	options.process(argc, argv);
	MidiRoll midiroll;
	if (options.getArgCount() == 0) {
		midiroll.read(cin);
		processMidiFile(midiroll, options);
	} else {
		midiroll.read(options.getArg(1));
		processMidiFile(midiroll, options);
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////

//////////////////////////////
//
// processMidiFile --
//

void processMidiFile(MidiRoll& rollfile, Options& options) {
	bool sustainQ = options.getBoolean("sustain-pedal");
	bool softQ    = options.getBoolean("soft-pedal");

	rollfile.joinTracks();
	rollfile.doTimeAnalysis();

	if (options.getBoolean("treble-volume")) {
		extractNoteVolumes(rollfile, 2);
		return;
	} else if (options.getBoolean("bass-volume")) {
		extractNoteVolumes(rollfile, 1);
		return;
	}

	// annotations encoded as labels
	for (int i=0; i<rollfile[0].getEventCount(); i++) {
		MidiEvent* me = &rollfile[0][i];
		int channel = me->getChannel();
		if (sustainQ) {
			if ((channel == 1) && me->isSustainOn()) {
				cout << me->seconds << "\t" << "Ped" << endl;
			} else if ((channel == 1) && me->isSustainOff()) {
				cout << me->seconds << "\t" << "*" << endl;
			}
		} if (softQ) {
			if ((channel == 1) && me->isSoftOn()) {
				cout << me->seconds << "\t" << "UC" << endl;
			} else if ((channel == 1) && me->isSoftOff()) {
				if (!sustainQ) {
					cout << me->seconds << "\t" << "*" << endl;
				} else {
					cout << me->seconds << "\t" << "**" << endl;
				}
			}
		}
	}
}



//////////////////////////////
//
// extractNoteVolumes --
//

void extractNoteVolumes(MidiRoll& rollfile, int track) {
	double lasttime = -1;
	for (int i=0; i<rollfile[0].getEventCount(); i++) {
		MidiEvent* me = &rollfile[0][i];
		if (me->track != track) {
			continue;
		}
		if (!me->isNoteOn()) {
			continue;
		}
		if (me->seconds == lasttime) {
			continue;
		}
		cout << me->seconds << "\t" << me->getP2() << endl;
	}
}



