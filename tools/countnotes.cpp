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
	options.define("t|time-of-first=b", "Display time of first note (as percent of total time)");
	options.define("T|average-time-of-first=b", "Display average time of first note");
	options.define("f|display-filename=b", "Display filename (for average-time-of-first)");
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
	vector<int> firsttick(128, -1);
	int duration = 0;
	if (options.getBoolean("time-of-first") || options.getBoolean("average-time-of-first")) {
		duration = rollfile.getFileDurationInTicks();
	}
	for (int i=0; i<rollfile.getTrackCount(); i++) {
		MidiEventList* mel = &rollfile[i];
		for (int j=0; j<mel->getEventCount(); j++) {
			MidiEvent* me = &(*mel)[j];
			if (me->isNoteOn()) {
				int key = me->getKeyNumber();
				keycount[key]++;
				if (firsttick[key] == -1) {
					firsttick[key] = me->tick;
				}
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
	} else if (options.getBoolean("average-time-of-first")) {
		double sum = 0.0;
		int counter = 0;
		for (int i=0; i<(int)keycount.size(); i++) {
			sum += firsttick[i]/(double)duration;
			counter++;
		}
		if (options.getBoolean("display-filename")) {
			cout << rollfile.getFilename() << "\t";
		}
		cout << sum/counter  << endl;
	} else {
		// List counts of MIDI files by default:
		for (int i=0; i<(int)keycount.size(); i++) {
			cout << i << ":\t" << keycount[i];
			if (duration) {
				cout << "\t" << (int)(firsttick[i]/(double)duration * 100.0 + 0.5);
			}
			cout << endl;
		}
	}
}



