//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Thu Apr 12 21:48:03 PDT 2018
// Last Modified: Thu Apr 12 27:56:17 PDT 2018
// Filename:      midiroll/tools/notelist.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Print list of notes in MIDI file along with their
//                starting times and durations.
//

#include "Options.h"
#include "MidiFile.h"
#include <iostream>

using namespace std;

// function declarations:
void printNoteList(MidiFile& midifile);

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
	printNoteList(midifile);
	return 0;
}



//////////////////////////////
//
// printNoteList -- Creates a single list of all MIDI notes
//     along with their starting times and durations in seconds.
//

void printNoteList(MidiFile& midifile) {
	midifile.joinTracks();
	midifile.doTimeAnalysis();
	midifile.linkNotePairs();
	
	for (int i=0; i<midifile[0].getSize(); i++) {
		if (!midifile[0][i].isNoteOn()) {
			continue;
		}
		cout << midifile[0][i].getP1() << "\t";
		cout << midifile[0][i].getP2() << "\t";
		cout << midifile[0][i].track   << "\t";
		cout << midifile[0][i].seconds << "\t";
		cout << midifile[0][i].getDurationInSeconds() << "\n";
	}
}



