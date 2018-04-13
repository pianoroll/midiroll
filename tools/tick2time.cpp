//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Thu Apr 12 21:10:03 PDT 2018
// Last Modified: Thu Apr 12 24:28:12 PDT 2018
// Filename:      midiroll/tools/tick2time.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Print tick to time mappings for a MIDI file.
//                Tempo changes will be processes when calculating
//                time.
//

#include "Options.h"
#include "MidiFile.h"
#include <iostream>

using namespace std;

// function declarations:
void printTickToTempoMap(MidiFile& midifile);

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.process(argc, argv);
	MidiFile midifile;
	if (options.getArgCount() == 0) {
		midifile.read(cin);
	} else if (options.getArgCount() == 1) {
		midifile.read(options.getArg(1));
	} else {
		cerr << "Usage: " << options.getCommand() << "  [midifile]" << endl;
		exit(1);
	}
	printTickToTempoMap(midifile);
	return 0;
}



//////////////////////////////
//
// printTickToTempoMap -- Creates a single list of all MIDI events
//     (by calling .joinTracks()) and then iterates through the list
//     of events printing unique tick values and their corresponding
//     performance time in seconds.  The .doTimeAnalysis() function
//     calculates the times in seconds.
//

void printTickToTempoMap(MidiFile& midifile) {
	midifile.joinTracks();
	midifile.doTimeAnalysis();
	int lasttick = -1;
	for (int i=0; i<midifile[0].getSize(); i++) {
		if (lasttick == midifile[0][i].tick) {
			continue;
		}
		cout << midifile[0][i].tick << "\t";
		cout << midifile[0][i].seconds << endl;
		lasttick = midifile[0][i].tick;
	}
}



