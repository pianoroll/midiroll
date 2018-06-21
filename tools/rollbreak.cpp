//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed Jun 20 15:40:22 PDT 2018
// Last Modified: Wed Jun 20 15:40:25 PDT 2018
// Filename:      midiroll/tools/setbreak.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Change the break point between left and right registers
//                for notes on a piano roll.
//
// Options:
//      rollbreak -b 65.5
//           Set the break between 65 (bass register) and 66 (treble register)
//      rollbreak -l file.mid [file2.mid ...]
//           List the the break point of the midifile.
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>

using namespace std;
using namespace smf;

// function declarations:
void    displayTempo        (Options& options);
void    setTempo            (Options& options);
void    applyTempoFactor    (MidiRoll& midiroll, double factor);
void    displayRegisterBreak(Options& options);
void    displayRegisterBreak(MidiRoll& midiroll);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("l|list=b", "list roll tempo(s)");
	options.define("b|break=d:65.5", "boundary between treble and bass notes");
	options.process(argc, argv);

	if (options.getBoolean("list")) {
		displayRegisterBreak(options);
	} else if (options.getBoolean("break")) {
		setTempo(options);
	} else {
		cerr << "Usage: " << options.getCommand()
		     << " [-b #|-l] midiin.mid [midiout.mid]"
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
// displayRegisterBreak -- list the breaks of the input MIDI file(s)
//

void displayRegisterBreak(Options& options) {
	MidiRoll midiroll;

	// display the register breaks of each MIDI file
	if (options.getArgCount() == 0) {
		midiroll.read(cin);
		displayRegisterBreak(midiroll);
	} else {
		for (int i=0; i<options.getArgCount(); i++) {
			midiroll.read(options.getArg(i+1));
			if (options.getArgCount() > 1) {
				cout << options.getArg(i+1) << '\t';
			}
			displayRegisterBreak(midiroll);
		}
	}
}



//////////////////////////////
//
// displayRegisterBreak --
//

void displayRegisterBreak(MidiRoll& midiroll) {
	vector<pair<int, int>> trackrange;
	trackrange.resize(midiroll.getTrackCount());
	for (int i=0; i<midiroll.getTrackCount(); i++) {
		trackrange[i].first = -1;
		trackrange[i].second = -1;
		for (int j=0; j<midiroll[i].getEventCount(); j++) {
			if (!midiroll[i][j].isNoteOn()) {
				continue;
			}
			int pitch = midiroll[i][j].getKeyNumber();
			if ((trackrange[i].first < 0) || (pitch < trackrange[i].first)) {
				trackrange[i].first = pitch;
			}
			if ((trackrange[i].second < 0) || (pitch > trackrange[i].second)) {
				trackrange[i].second = pitch;
			}
		}
	}

	cout << "TRK\tMIN\tMAX\tNOTE\n";
	for (int i=0; i<(int)trackrange.size(); i++) {
		cout << i << "\t";
		cout << trackrange[i].first << "\t";
		cout << trackrange[i].second << "\t";
		switch (i) {
			case 0: cout << "acceleration emulation"; break;
			case 1: cout << "bass notes"; break;
			case 2: cout << "treble notes"; break;
			case 3: cout << "bass expression"; break;
			case 4: cout << "treble expression"; break;
		}
		cout << endl;
	}

}


