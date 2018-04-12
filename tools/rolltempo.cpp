//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed Apr 11 16:24:24 PDT 2018
// Last Modified: Wed Apr 11 16:41:57 PDT 2018
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
#include "MidiFile.h"
#include <iostream>

using namespace std;

// function declarations:
void    displayTempo     (Options& options);
void    setTempo         (Options& options);

// The mutliplier to convert from tempo to ticks:
// The ticks-per-quarter note are 6 times the tempo.
// One tick represents one image pixel row.  The image is
// scanned at 300 dpi (measured actually to be about 300.25 dpi).
// A tempo of 100 means the roll moves (at its start) by
// 10.0 feet per minute.  This is 10.0 * 300 * 12 = 36000 rows/minute.
// A reference tempo of 60 bpm is used, so each "quarter note" is
// 36000 / 60 = 600 rows.
const double factor = 6.0;

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
	int tpq = int(factor * options.getDouble("tempo") + 0.5);
	if (tpq < 1)      { tpq = 1; }
	if (tpq > 0x7fff) { tpq = 0x7fff; }

	MidiFile midifile;
	if (options.getArgCount() == 0) {
		// Read from standard input and write to standard output:
		midifile.read(cin);
		midifile.setTicksPerQuarterNote(tpq);
		cout << midifile;
	} else {
		for (int i=0; i < options.getArgCount(); i++) {
			if (i > 1) {
				cerr << "Processing file " << options.getArgCount() << endl;
			}
			midifile.read(options.getArg(i+1));
			midifile.setTicksPerQuarterNote(tpq);
			midifile.write(options.getArg(i+1));
		}
	}
}



//////////////////////////////
//
// displayTempo -- list the tempo of the input MIDI file(s).  If there
//     is more than one file, the filename will also be printed.
//

void displayTempo(Options& options) {
	MidiFile midifile;

	// display the tempos of each MIDI file
	if (options.getArgCount() == 0) {
		midifile.read(cin);
		cout << midifile.getTicksPerQuarterNote() / factor << endl;
	} else {
		for (int i=0; i<options.getArgCount(); i++) {
			midifile.read(options.getArg(i+1));
			if (options.getArgCount() > 1) {
				cout << options.getArg(i+1) << '\t';
			}
			cout << midifile.getTicksPerQuarterNote() / factor << endl;
		}
	}
}



