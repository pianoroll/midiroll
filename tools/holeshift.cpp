//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri Apr 12 11:24:53 EDT 2019
// Last Modified: Fri Apr 12 11:24:55 EDT 2019
// Filename:      midiroll/tools/holeshift.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Transpose MIDI file when the hole positions are off
//                from the correct position for some reason when extracting
//                from the roll images.
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <sys/stat.h>

using namespace std;
using namespace smf;

// function declarations:
void  processMidiFile     (MidiRoll& rollfile, Options& options);
void  applyRegisterSplits (MidiRoll& rollfile, int bass, int treble, int trebleExp);
int   getRegion           (int key, int bass, int treble, int trebleExp);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("t|transpose=i:0", "Transpose by pitch amount");
	options.define("r|red|t100|T100|t-100|T-100|red-roll=b", "Split into four registers of red roll");
	options.define("g|green|t98|T98|t-98|T-98|green-roll=b", "Split into four registers of green roll");
	options.define("replace=b", "overwrite the input data with the output data");
	options.process(argc, argv);
	MidiRoll midiroll;
	if (options.getArgCount() == 0) {
		midiroll.read(cin);
		processMidiFile(midiroll, options);
		midiroll.write(cout);
	} else if (options.getArgCount() == 1) {
		midiroll.read(options.getArg(1));
		processMidiFile(midiroll, options);
		if (options.getBoolean("replace")) {
			midiroll.write(options.getArg(1));
		} else {
			midiroll.write(cout);
		}
	} else if (!options.getBoolean("replace")) {
		// two arguments: input file and output filename
		midiroll.read(options.getArg(1));
		processMidiFile(midiroll, options);
		// check to see if file exists and don't overwrite
		struct stat buffer;
		if (stat(options.getArg(2).c_str(), &buffer) == 0) {
			cerr << "Error: cannot overwrite " << options.getArg(2) << endl;
		} else {
			midiroll.write(options.getArg(2));
		}
	} else {
		for (int i=0; i<options.getArgCount(); i++) {
			midiroll.read(options.getArg(i+1));
			processMidiFile(midiroll, options);
			midiroll.write(options.getArg(i+1));
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

	// First transpose all key numbers by the specified transposition.
	int transpose = options.getInteger("transpose");
	if (transpose == 0) {
		cerr << "Nothing to do since transpose value is 0" << endl;
		exit(1);
	}
	
	for (int i=0; i<rollfile.getTrackCount(); i++) {
		MidiEventList* mel = &rollfile[i];
		for (int j=0; j<mel->getEventCount(); j++) {
			MidiEvent* me;
			me = &mel->getEvent(j);
			if (!me->isNote()) {
				continue;
			}
			int key = me->getKeyNumber();
			key += transpose;
			me->setKeyNumber(key);

		}
	}

	// Now shift the notes to different tracks/channels if they
	// change registers or move in/out of expression tracks.
	// Currently only for red and green Welte rolls.
	// See RollOptions.cpp source code for where the numbers
	// came from.
	if (options.getBoolean("green")) {
		// 0 = always lower region of bass expression.
		// 21 = first note of bass-note region.
		// 67 = first note of treble-note region (G4)
		// 109 = first note of treble-expression.
		applyRegisterSplits(rollfile, 21, 67, 109);
	} else {
		// red rolls are the default, but maybe make them
		// not be the default (pianola being the default?).

		// 0 = always lower region of bass expression.
		// 24 = first note of bass-note region.
		// 67 = first note of treble-note region (G4)
		// 104 = first note of treble-expression.
		applyRegisterSplits(rollfile, 24, 67, 104);
	}

	rollfile.setMetadata("HOLE_SHIFT", "\t\t" + to_string(transpose));
}



//////////////////////////////
//
// applyRegisterSplits --
//    track 0: no notes
//    track 1: bass notes, channel 1
//    track 2: treble notes, channel 2
//    track 3: bass expression channel 0
//    track 4: treble expression, channel 3
//

void applyRegisterSplits(MidiRoll& rollfile, int bass, int treble, int trebleExp) {

	vector<int> regiontrack(4);
	regiontrack[0] = 3;  // bass expression
	regiontrack[1] = 1;  // bass notes
	regiontrack[2] = 2;  // treble notes
	regiontrack[3] = 4;  // treble expression

	vector<int> regionchannel(4);
	regionchannel[0] = 0; // bass expression
	regionchannel[1] = 1; // bass notes
	regionchannel[2] = 2; // treble notes
	regionchannel[3] = 3; // treble expression
	
	rollfile.joinTracks();

	for (int i=0; i<rollfile.getTrackCount(); i++) {
		MidiEventList* mel = &rollfile[i];
		for (int j=0; j<mel->getEventCount(); j++) {
			MidiEvent* me = &mel->getEvent(j);
			if (!me->isNote()) {
				continue;
			}
			int key = me->getKeyNumber();
			int region = getRegion(key, bass, treble, trebleExp);
			int channel = me->getChannel();
			if (regionchannel[region] != channel) {
				me->setChannel(regionchannel[region]);
			}
			if (regiontrack[region] != me->track) {
				me->track = regiontrack[region];
			}
		}
	}

	rollfile.splitTracks();
	rollfile.sortTracks();
}



//////////////////////////////
//
// getRegion --
//

int getRegion(int key, int bass, int treble, int trebleExp) {
	if (key < bass) {
		return 0;
	} else if (key < treble) {
		return 1;
	} else if (key < trebleExp) {
		return 2;
	} else {
		return 3;
	}
}



