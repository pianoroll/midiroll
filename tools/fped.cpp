//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat Apr 20 08:06:49 EDT 2019
// Last Modified: Sat Apr 20 09:10:12 EDT 2019
// Filename:      midiroll/tools/fped.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Finger pedalling: convert sustain pedal into note extensions.
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
	rollfile.joinTracks();
	vector<vector<MidiEvent*>> offs(16);
	vector<bool> sustain(16);

	for (int i=0; i<rollfile[0].getEventCount(); i++) {
		MidiEvent* me = &rollfile[0][i];
		int channel = me->getChannel();
		if (me->isSustainOn()) {
			sustain[channel] = true;
			me->clear();
			continue;
		}
		if (me->isSustainOff()) {
			sustain[channel] = false;
			for (int j=0; j<(int)offs[channel].size(); j++) {
				offs[channel][j]->tick = me->tick;
			}
			offs[channel].clear();
			me->clear();
			continue;
		}
		if (!sustain[channel]) {
			continue;
		}
		if (me->isNoteOff()) {
			offs[channel].push_back(me);
		}
	}

	rollfile.splitTracks();
	rollfile.removeEmpties();
	rollfile.sortTracks();
}



