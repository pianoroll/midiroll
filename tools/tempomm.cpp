//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Jun 19 17:32:10 PDT 2018
// Last Modified: Tue Jun 19 17:32:13 PDT 2018
// Filename:      midiroll/tools/tempomm.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Change the ticks-per-quarter-note value in the MIDI header
//                to match a specific piano roll tempo.  Also can display
//                the tempo of a MIDI file.
//
// Options:
//      tempomm -f 1.5 file.mid [file2.mid ...]
//           Change tempos in the MIDI file by a factor of 1.5.
//      rolltempo -l file.mid [file2.mid ...]
//           List the tempo metamessages in the file.
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>

using namespace std;
using namespace smf;

// function declarations:
void    displayTempo     (Options& options);
void    setTempo         (Options& options);
void    applyTempoFactor (MidiRoll& midiroll, double factor);
void    displayMidiTempos(MidiRoll& midiroll);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("l|list=b", "list roll tempo(s)");
	options.define("f|factor=d:1.0", "tempo scaling factor");
	options.process(argc, argv);

	if (options.getBoolean("list")) {
		displayTempo(options);
	} else if (options.getBoolean("factor")) {
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
	double factor = options.getDouble("factor");
	if (factor <= 0.0) {
		factor = 1;
	}
	if (options.getArgCount() == 0) {
		// Read from standard input and write to standard output:
		midiroll.read(cin);
		applyTempoFactor(midiroll, factor);
		cout << midiroll;
	} if (options.getArgCount() == 1) {
		// Read from a file input and write to a standard output
		midiroll.read(options.getArg(1));
		applyTempoFactor(midiroll, factor);
		cout << midiroll;
	} if (options.getArgCount() == 2) {
		midiroll.read(options.getArg(1));
		applyTempoFactor(midiroll, factor);
		midiroll.write(options.getArg(2));
	} else {
		cerr << "Usage: " << options.getCommand() 
		     << " [-l|-f #] input.mid [output.mid]" << endl;
	}
}



//////////////////////////////
//
// applyTempoFactor --
//

void applyTempoFactor(MidiRoll& midiroll, double factor) {
	for (int i=0; i<midiroll[0].getEventCount(); i++) {
		if (!midiroll[0][i].isTempo()) {
			continue;
		}
		int microsec = midiroll[0][i].getTempoMicroseconds();
		microsec = int(microsec / factor + 0.5);
		midiroll[0][i].setTempoMicroseconds(microsec);
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
		displayMidiTempos(midiroll);
	} else {
		for (int i=0; i<options.getArgCount(); i++) {
			midiroll.read(options.getArg(i+1));
			if (options.getArgCount() > 1) {
				cout << options.getArg(i+1) << '\t';
			}
			displayMidiTempos(midiroll);
		}
	}
}


//////////////////////////////
//
// displayMidiTempos --
//

void displayMidiTempos(MidiRoll& midiroll) {
	for (int i=0; i<midiroll[0].getEventCount(); i++) {
		if (!midiroll[0][i].isTempo()) {
			continue;
		}
		cout << midiroll[0][i].getTempoBPM() << endl;
	}
	cout << endl;
}


