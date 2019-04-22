//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon Apr 22 10:28:31 EDT 2019
// Last Modified: Mon Apr 22 10:32:14 EDT 2019
// Filename:      midiroll/tools/roll2smpte.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Convert roll with tempo changes (every foot) into SMPTE timings without
//                tempo changes, using 25 frames/sec. and 40 subframes/frame.  This makes
//                each tick 1 millisecond long (Tempo 60 kepts in track 0 to ensure this).
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
	rollfile.doTimeAnalysis();
	rollfile.setMillisecondTicks();
	bool deletetempo = false;

	for (int i=0; i<rollfile[0].getEventCount(); i++) {
		MidiEvent* me = &rollfile[0][i];
		me->tick = (int)(me->seconds * 1000.0 + 0.5);
		if (me->isTempo()) {
			if (deletetempo) {
				me->clear();
			} else {
				deletetempo = true;
			}
		}
	}

	rollfile.splitTracks();
	rollfile.removeEmpties();
	rollfile.sortTracks();  // maybe not be needed
}



