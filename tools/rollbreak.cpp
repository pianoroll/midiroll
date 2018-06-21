//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed Jun 20 15:40:22 PDT 2018
// Last Modified: Wed Jun 20 20:26:13 PDT 2018
// Filename:      midiroll/tools/setbreak.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Change the break point between left and right registers
//                for notes on a piano roll.
//
// Options:
//      rollbreak -b 66.5
//           Set the break between 65 (bass register) and 66 (treble register)
//      rollbreak -g 
//           Set the break between 65 (bass register) and 66 (treble register)
//           (-g means "green roll")
//      rollbreak -l file.mid [file2.mid ...]
//           List the the break point of the midifile.
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>

using namespace std;
using namespace smf;

// function declarations:
void  displayTempo          (Options& options);
void  setBreak              (Options& options);
void  applyBreakpoint       (MidiRoll& midiroll, Options& options);
void  displayRegisterBreak  (Options& options);
void  displayRegisterBreak  (MidiRoll& midiroll, int count);
void  errorMessage          (Options& options, const string& message = "");


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("l|list=b", "list roll tempo(s)");
	options.define("b|break=d:66.5", "boundary between treble and bass notes");
	options.define("w|welte-mignon=b", "boundary between treble and bass notes is 66.5");
	options.define("green-welte=b", "boundary between treble and bass notes is 66.5");
	options.define("red-welte=b", "boundary between treble and bass notes is 66.5");
	options.define("licensee-welte=b", "boundary between treble and bass notes is 66.5");
	options.define("a|ampico=b", "boundary between treble and bass notes is 64.5");
	options.define("d|duo-art=b", "boundary between treble and bass notes is 63.5");
	options.define("treble-track=i:2", "track containing treble notes");
	options.define("bass-track=i:1", "track containing bass notes");
	options.process(argc, argv);

	if (options.getBoolean("list")) {
		displayRegisterBreak(options);
	} else {
		setBreak(options);
	}

	return 0;
}



//////////////////////////////
//
// setBreak -- Set the tempo of the MIDI file(s).  The tempo is
//    controlled by the ticks-per-quarter value in the MIDI header
//    rather than a tempo meta message.  The tempo meta messages
//    are instead used to control an emulation of the roll
//    acceleration over time.
//

void setBreak(Options& options) {
	MidiRoll midiroll;

	if (options.getArgCount() == 0) {
		// Read from standard input and write to standard output:
		midiroll.read(cin);
		applyBreakpoint(midiroll, options);
		cout << midiroll;
	} if (options.getArgCount() == 1) {
		// Read from a file input and write to a standard output
		midiroll.read(options.getArg(1));
		applyBreakpoint(midiroll, options);
		cout << midiroll;
	} if (options.getArgCount() == 2) {
		midiroll.read(options.getArg(1));
		applyBreakpoint(midiroll, options);
		midiroll.write(options.getArg(2));
	} else {
		errorMessage(options);
	}
}



//////////////////////////////
//
// applyBreakpoint --
//

void applyBreakpoint(MidiRoll& midiroll, Options& options) {
	double breakpoint = options.getDouble("break");
	if (options.getBoolean("green-welte")) {
		breakpoint = 66.5;
	}
	if (options.getBoolean("red-welte")) {
		breakpoint = 66.5;
	}
	if (options.getBoolean("licensee-welte")) {
		breakpoint = 66.5;
	}
	if (options.getBoolean("welte-mignon")) {
		breakpoint = 66.5;
	}
	if (options.getBoolean("ampico")) {
		breakpoint = 64.5;
	}
	if (options.getBoolean("duo-art")) {
		breakpoint = 63.5;
	}

	if ((breakpoint < 20) || (breakpoint > 80)) {
		errorMessage(options, "Invalid breakpoint");
		exit(1);
	}

	int trebletrack = options.getInteger("treble-track");
	int basstrack = options.getInteger("bass-track");

	midiroll.joinTracks();
	for (int i=0; i<midiroll[0].getEventCount(); i++) {
		if (!midiroll[0][i].isNote()) {
			continue;
		}
		int key = midiroll[0][i].getKeyNumber();
		int track = midiroll[0][i].track;
		if (track == trebletrack) {
			if (key < breakpoint) {
				midiroll[0][i].track = basstrack;
			}
		}
		if (track == basstrack) {
			if (key > breakpoint) {
				midiroll[0][i].track = trebletrack;
			}
		}
	}
	
	midiroll.splitTracks();
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
		displayRegisterBreak(midiroll, 1);
	} else {
		for (int i=0; i<options.getArgCount(); i++) {
			midiroll.read(options.getArg(i+1));
			if (options.getArgCount() > 1) {
				cout << options.getArg(i+1) << endl;
			}
			displayRegisterBreak(midiroll, options.getArgCount());
		}
	}
}



//////////////////////////////
//
// displayRegisterBreak --
//

void displayRegisterBreak(MidiRoll& midiroll, int count) {
	vector<pair<int, int>> trackrange;
	string prefix;
	if (count > 1) {
		prefix = "\t";
	}
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

	int minn = -1;
	int maxx = -1;
	for (int i=0; i<(int)trackrange.size(); i++) {
		if (trackrange[i].first >= 0) {
			if ((minn < 0) || (minn > trackrange[i].first)) {
				minn = trackrange[i].first;
			}
		}
		if (trackrange[i].second >= 0) {
			if (maxx < trackrange[i].second) {
				maxx = trackrange[i].second;
			}
		}
	}

	cout << prefix << "TRK\tMIN\tMAX\tNOTE\n";
	for (int i=0; i<(int)trackrange.size(); i++) {
		cout << prefix << i << "\t";
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
	cout << prefix << "all" << "\t" << minn << "\t" << maxx << "\t";
	cout << "range: " << (maxx - minn) + 1 << " notes" << endl;

}



//////////////////////////////
//
// errorMessage -- print usage message
//

void errorMessage(Options& options, const string& message) {
	if (message.empty()) {
		cerr << "Usage: " << options.getCommand()
		     << " [-w|-a|-d|-b #|-l] midiin.mid [midiout.mid]"
		     << endl;
	} else {
		cerr << "ERROR: " << message << endl;
	}
}



