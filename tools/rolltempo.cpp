//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed Apr 11 16:24:24 PDT 2018
// Last Modified: Fri Apr 13 08:13:45 PDT 2018
// Filename:      midiroll/tools/rolltempo.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Change the ticks-per-quarter-note value in the MIDI header
//                to match a specific piano roll tempo.  Also can display
//                the tempo of a MIDI file.
//
// Options:
//      rolltempo -t 90 file.mid [file2.mid ...]
//           Change tempo to 90.
//      rolltempo -l file.mid [file2.mid ...]
//           List the tempo of the file.
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>

using namespace std;

// function declarations:
void    displayTempo     (Options& options);
void    setTempo         (Options& options);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("l|list=b", "list roll tempo(s)");
	options.define("t|tempo=i:85", "roll tempo");
	options.process(argc, argv);

	if (options.getBoolean("list")) {
		displayTempo(options);
	} else if (options.getBoolean("tempo")) {
		setTempo(options);
	} else {
		cerr << "Usage: " << options.getCommand()
		     << " [-t #|-l] midifile(s)"
		     << endl;
	}

	return 0;
}



//////////////////////////////
//
// setTempo -- Set the tempo of the MIDI file(s).  The tempo is
//    controlled by the ticks-per-quarter value in the MIDI header
//    rather than a tempo meta message.  The tempo meta messages
//    are instead used to control an emulation of the roll
//    acceleration over time.
//

void setTempo(Options& options) {
	MidiRoll midiroll;
	if (options.getArgCount() == 0) {
		// Read from standard input and write to standard output:
		midiroll.read(cin);
		midiroll.setRollTempo(options.getDouble("tempo"));
		cout << midiroll;
	} else {
		for (int i=0; i < options.getArgCount(); i++) {
			if (i > 1) {
				cerr << "Processing file " << options.getArgCount() << endl;
			}
			midiroll.read(options.getArg(i+1));
			midiroll.setRollTempo(options.getDouble("tempo"));
			midiroll.write(options.getArg(i+1));
		}
	}
}



//////////////////////////////
//
// displayTempo -- list the tempo of the input MIDI file(s).  If there
//     is more than one file, the filename will also be printed.
//

void displayTempo(Options& options) {
	MidiRoll midiroll;

	// display the tempos of each MIDI file
	if (options.getArgCount() == 0) {
		midiroll.read(cin);
		cout << midiroll.getRollTempo() << endl;
	} else {
		for (int i=0; i<options.getArgCount(); i++) {
			midiroll.read(options.getArg(i+1));
			if (options.getArgCount() > 1) {
				cout << options.getArg(i+1) << '\t';
			}
			cout << midiroll.getRollTempo() << endl;
		}
	}
}



