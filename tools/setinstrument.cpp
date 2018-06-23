//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri Jun 22 13:07:53 PDT 2018
// Last Modified: Fri Jun 22 17:06:41 PDT 2018
// Filename:      midiroll/tools/setinstrument.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Add or change the instrument numbers in all tracks.
//
// Options:
//      setinstrument -i 0
//           Set the instrument in all parts to timbre 0
//      setinstrument -p 
//           Set the instrument in all part to piano (timbre 0)
//      setinstrument -l file.mid [file2.mid ...]
//           List the instruments in the midifile.
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>

using namespace std;
using namespace smf;

// function declarations:
void  displayTempo           (Options& options);
void  setInstrument          (Options& options);
void  setInstrumentOverwrite (Options& options);
void  setInstrument          (MidiRoll& midiroll, Options& options);
void  displayInstruments     (Options& options);
void  displayInstruments     (MidiRoll& midiroll, Options& options, int count);
void  errorMessage           (Options& options, const string& message = "");
void  getPatchChanges        (vector<vector<MidiEvent*>>& patchchanges, MidiRoll& midiroll);
void  getTrackNoteChannels   (vector<int>& channels, MidiRoll& midiroll);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("l|list=b",         "list roll tempo(s)");
	options.define("i|instrument=i:0", "set all tracks to the given instrument number");
	options.define("p|piano=b",        "use piano timbre for all tracks");
	options.define("r|remove=b",       "remove all timbre MIDI messages.");
	options.define("replace=b",        "replace contents of input files");
	options.process(argc, argv);

	if (options.getBoolean("list")) {
		displayInstruments(options);
	} else if (options.getBoolean("replace")) {
		setInstrumentOverwrite(options);
	} else {
		setInstrument(options);
	}

	return 0;
}



//////////////////////////////
//
// setInstrument -- Set the tempo of the MIDI file(s).  The tempo is
//    controlled by the ticks-per-quarter value in the MIDI header
//    rather than a tempo meta message.  The tempo meta messages
//    are instead used to control an emulation of the roll
//    acceleration over time.
//

void setInstrument(Options& options) {
	MidiRoll midiroll;

	if (options.getArgCount() == 0) {
		// Read from standard input and write to standard output:
		midiroll.read(cin);
		setInstrument(midiroll, options);
		cout << midiroll;
	} if (options.getArgCount() == 1) {
		// Read from a file input and write to a standard output
		midiroll.read(options.getArg(1));
		setInstrument(midiroll, options);
		cout << midiroll;
	} if (options.getArgCount() == 2) {
		midiroll.read(options.getArg(1));
		setInstrument(midiroll, options);
		midiroll.write(options.getArg(2));
	} else {
		errorMessage(options);
	}
}



//////////////////////////////
//
// setInstrumentOverwrite -- Same as setInstrument, but will replace
//    input MIDI file.
//

void setInstrumentOverwrite(Options& options) {
	MidiRoll midiroll;
	if (options.getArgCount() == 0) {
		errorMessage(options);
	} else {
		for (int i=0; i<options.getArgCount(); i++) {
			midiroll.read(options.getArg(i+1));
			setInstrument(midiroll, options);
			midiroll.write(options.getArg(i+1));
		}
	}
}



//////////////////////////////
//
// setInstrument --
//

void setInstrument(MidiRoll& midiroll, Options& options) {
	vector<vector<MidiEvent*>> patchchanges;
	getPatchChanges(patchchanges, midiroll);

	vector<int> channels;
	getTrackNoteChannels(channels, midiroll);

	int pc = options.getInteger("instrument");
	if (options.getBoolean("piano")) {
		pc = 0;
	}
	if (pc < 0) {
		pc = 0;
	}
	if (pc > 127) {
		pc = 127;
	}

	bool needsSort = false;

	for (int i=0; i<midiroll.getTrackCount(); i++) {
		if (!patchchanges[i].empty()) {
			for (int j=0; j<(int)patchchanges[i].size(); j++) {
				patchchanges[i][j]->setP1(pc);
			}
		} else if (channels[i] >= 0) {
			// need to add a patch change
			midiroll.addPatchChange(i, 0, channels[i], pc);
			needsSort = true;
		}
	}

	if (needsSort) {
		midiroll.sortTracks();
	}
}



//////////////////////////////
//
// getTrackNoteChannels --
//

void getTrackNoteChannels(vector<int>& channels, MidiRoll& midiroll) {
	channels.resize(midiroll.getTrackCount());
	for (int i=0; i<midiroll.getTrackCount(); i++) {
		channels[i] = -1;
		for (int j=0; j<midiroll[i].getEventCount(); j++) {
			if (!midiroll[i][j].isNote()) {
				continue;
			}
			channels[i] = midiroll[i][j].getChannel();
			break;
		}
	}
}



//////////////////////////////
//
// displayInstruments -- list the breaks of the input MIDI file(s)
//

void displayInstruments(Options& options) {
	MidiRoll midiroll;

	// display the instrument messages in each MIDI file
	if (options.getArgCount() == 0) {
		midiroll.read(cin);
		displayInstruments(midiroll, options, 1);
	} else {
		for (int i=0; i<options.getArgCount(); i++) {
			midiroll.read(options.getArg(i+1));
			if (options.getArgCount() > 1) {
				cout << options.getArg(i+1);
				cout << "\t";
			}
			displayInstruments(midiroll, options, options.getArgCount());
		}
	}
}



//////////////////////////////
//
// getPatchChanges --
//

void getPatchChanges(vector<vector<MidiEvent*>>& patchchanges, MidiRoll& midiroll) {
	patchchanges.resize(0);
	patchchanges.resize(midiroll.getTrackCount());
	for (int i=0; i<midiroll.getTrackCount(); i++) {
		patchchanges[i].resize(0);
		for (int j=0; j<midiroll[i].getEventCount(); j++) {
			if (!midiroll[i][j].isTimbre()) {
				continue;
			}
			patchchanges[i].push_back(&midiroll[i][j]);
		}
	}
}



//////////////////////////////
//
// displayInstruments --
//

void displayInstruments(MidiRoll& midiroll, Options& options, int count) {
	vector<vector<MidiEvent*>> patchchanges;
	getPatchChanges(patchchanges, midiroll);

	string prefix;
	if (count > 1) {
		prefix = "\t";
	}
	if (count > 1) {
		cout << endl;
	}

	for (int i=0; i<(int)patchchanges.size(); i++) {
		cout << prefix << "track " << i << ":\t";
		if (patchchanges[i].size() > 0) {
			for (int j=0; j<(int)patchchanges[i].size(); j++) {
				cout << "t" << patchchanges[i][j]->tick;
				cout << ":" << patchchanges[i][j]->getP1();
				if (j < (int)patchchanges[i].size() - 1) {
					cout << ", ";
				}
			}
		} else {
			cout << "none";
		}
		cout << endl;
	}
}



//////////////////////////////
//
// errorMessage -- print usage message
//

void errorMessage(Options& options, const string& message) {
	if (message.empty()) {
		cerr << "Usage: " << options.getCommand()
		     << " [-i #|-p|-l] midiin.mid [midiout.mid]"
		     << endl;
	} else {
		cerr << "ERROR: " << message << endl;
	}
}



