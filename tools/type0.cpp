//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon May  6 17:09:18 PDT 2019
// Last Modified: Mon May  6 17:09:20 PDT 2019
// Filename:      midiroll/tools/type0.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Convert a MIDI roll into a type-0 MIDI file.
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>
#include <string>

using namespace std;
using namespace smf;

// function declarations:
int  processMidiFile                (MidiRoll& rollfile, Options& options);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("s|sustain=b",    "Make sustain pedal more shallow.");
	options.define("c|channel=i:0",  "Move notes to given channel (zero offset).");
	options.define("M|no-meta=b",    "No meta-messages in output");
	options.define("X|no-text=b",    "No text meta-messages in output");
	options.define("T|no-tempo=b",   "No tempo meta-messages in output");
	options.define("P|no-panning=b", "No panning");
	options.define("C|no-timbre=b",  "No timbre change messages");
	options.define("a|all=b",        "Equivalent to -sMTXPC -c0");
	options.process(argc, argv);

	MidiRoll midiroll;
	if (options.getArgCount() == 0) {
		midiroll.read(cin);
		processMidiFile(midiroll, options);
		cout << midiroll << endl;
	} else {
		midiroll.read(options.getArg(1));
		int status = processMidiFile(midiroll, options);
		if (status) {
			if (options.getArgCount() >= 2) {
				midiroll.write(options.getArg(2));
			} else {	
				midiroll.writeBinascWithComments(cout);
			}
		}
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////

//////////////////////////////
//
// processMidiFile --
//

int processMidiFile(MidiRoll& rollfile, Options& options) {
	rollfile.joinTracks();

	int targetChan = options.getInteger("channel");
	bool sustainQ  = options.getBoolean("sustain");
	bool tempoQ    = !options.getBoolean("no-tempo");
	bool textQ     = !options.getBoolean("no-text");
	bool metaQ     = !options.getBoolean("no-meta");
	bool panQ      = !options.getBoolean("no-panning");
	bool timbreQ   = !options.getBoolean("no-timbre");

	if (options.getBoolean("all")) {
		targetChan = 0;
		sustainQ = 1;
		tempoQ   = 0;
		textQ    = 0;
		metaQ    = 0;
		panQ     = 0;
		timbreQ  = 0;
	}

	if (!tempoQ) {
		rollfile.doTimeAnalysis();
		rollfile.setTPQ(1000);
	}

	for (int i=0; i<rollfile[0].getEventCount(); i++) {
		if (!tempoQ) {
			rollfile[0][i].tick = int(rollfile[0][i].seconds * 1000.0 * 2.0 + 0.5);
		}
		if ((!tempoQ) && rollfile[0][i].isTempo()) {
			rollfile[0][i].clear();
			continue;
		}
		if ((!textQ) && rollfile[0][i].isText()) {
			rollfile[0][i].clear();
			continue;
		}
		if ((!metaQ) && rollfile[0][i].isMeta()) {
			rollfile[0][i].clear();
			continue;
		}
		if ((!panQ) && rollfile[0][i].isController() && (rollfile[0][i].getP2() == 52)) {
			rollfile[0][i].clear();
			continue;
		}
		if ((!timbreQ) && rollfile[0][i].isTimbre()) {
			rollfile[0][i].clear();
			continue;
		}

		int channel = rollfile[0][i].getChannel();
		if (channel == 15) {
			// meta message or similar (channel 15 not otherwise used).
			continue;
		}

		if (sustainQ && rollfile[0][i].isSustainOn()) {
			rollfile[0][i].setP2(127);
		}
		if (sustainQ && rollfile[0][i].isSustainOff()) {
			rollfile[0][i].setP2(0);
		}

		if (channel >= 2 && !rollfile[0][i].isNote()) {
			rollfile[0][i].clear();
		} else {
			rollfile[0][i].setChannel(targetChan);
		}
	}

	rollfile.removeEmpties();
	return 1;
}



