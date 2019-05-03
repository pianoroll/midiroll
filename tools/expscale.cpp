//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri May  3 15:42:35 PDT 2019
// Last Modified: Fri May  3 16:46:23 PDT 2019
// Filename:      midiroll/tools/expscale.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Rescale the attack velocities of notes to a new range.
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>
#include <string>

using namespace std;
using namespace smf;

// function declarations:
int  processMidiFile                (MidiRoll& rollfile, Options& options);
void listAttackVelocities           (MidiRoll& rollfile);
int  getCurrentMinimumAttackVelocity(MidiRoll& rollfile);
int  getCurrentMaximumAttackVelocity(MidiRoll& rollfile);
int  mapToNewVelocity               (int velocity, double oldmin, double oldmax,
                                     double newmin, double newmax);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("old-min=d:-1.0", "old minimum value");
	options.define("old-max=d:-1.0", "old maximum value");
	options.define("n|min|new-min=d:-1.0", "new minimum value");
	options.define("x|max|new-max=d:-1.0", "new maximum value");
	options.define("l|list=b", "list all note attacks");
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
	if (options.getBoolean("list")) {
		listAttackVelocities(rollfile);
		return 0;
	}

	if (rollfile.getTrackCount() < 3) {
		cerr << "Error: There must be a minumum of 3 traks in the MIDI roll" << endl;
		exit(1);
	}

	double oldmin;
	double oldmax;

	if (options.getBoolean("old-min")) {
		oldmin = options.getDouble("old-min");
	} else {
		oldmin = getCurrentMinimumAttackVelocity(rollfile);
	}
	if (oldmin < 1) {
		oldmin = 1;
	}
	if (oldmin > 127) {
		oldmin = 127;
	}

	if (options.getBoolean("old-max")) {
		oldmax = options.getDouble("old-max");
	} else {
		oldmax = getCurrentMaximumAttackVelocity(rollfile);
	}
	if (oldmax < 1) {
		oldmax = 1;
	}
	if (oldmax > 127) {
		oldmax = 127;
	}

	if (!options.getBoolean("new-min")) {
		cout << "current range: " << oldmin << " to " << oldmax << endl;
		return 0;
	}
	if (!options.getBoolean("new-max")) {
		cout << "current range: " << oldmin << " to " << oldmax << endl;
		return 0;
	}

	double newmin = options.getDouble("new-min");
	if (newmin < 1) {
		newmin = 1;
	}
	if (newmin > 127) {
		newmin = 127;
	}
	double newmax = options.getDouble("new-max");
	if (newmax < 1) {
		newmax = 1;
	}
	if (newmax > 127) {
		newmax = 127;
	}

	for (int i=1; i<=2; i++) {
		for (int j=0; j<rollfile[i].getEventCount(); j++) {
			if (!rollfile[i][j].isNote()) {
				continue;
			}
			if (rollfile[i][j].isNoteOff()) {
				if (rollfile[i][j].getVelocity() == 0) {
					continue;
				}
			}
			int velocity = mapToNewVelocity(rollfile[i][j].getVelocity(), oldmin, oldmax, newmin, newmax);
			rollfile[i][j].setVelocity(velocity);
		}
	}
	return 1;
}



///////////////////////////////
//
// getCurrentMinimumAttackVelocity --
//

int getCurrentMinimumAttackVelocity(MidiRoll& rollfile) {
	int output = -1;

	for (int i=1; i<=2; i++) {
		for (int j=0; j<rollfile[i].getEventCount(); j++) {
			if (!rollfile[i][j].isNoteOn()) {
				continue;
			}
			if ((output < 0) || (rollfile[i][j].getVelocity() < output)) {
				output = rollfile[i][j].getVelocity();
			}
		}
	}
	return output;
}



///////////////////////////////
//
// getCurrentMaximumAttackVelocity --
//

int getCurrentMaximumAttackVelocity(MidiRoll& rollfile) {
	int output = -1;
	for (int i=1; i<=2; i++) {
		for (int j=0; j<rollfile[1].getEventCount(); j++) {
			if (!rollfile[i][j].isNoteOn()) {
				continue;
			}
			if ((output < 0) || (rollfile[i][j].getVelocity() > output)) {
				output = rollfile[i][j].getVelocity();
			}
		}
	}
	return output;
}



//////////////////////////////
//
// listAttackVelocities --
//

void listAttackVelocities(MidiRoll& rollfile) {
	for (int i=1; i<=2; i++) {
		for (int j=0; j<rollfile[i].getEventCount(); j++) {
			if (!rollfile[i][j].isNoteOn()) {
				continue;
			}
			cout << rollfile[i][j].getVelocity() << endl;
		}
	}
}



//////////////////////////////
//
// mapToNewVelocity -- Linear interpolation to new range.
//

int mapToNewVelocity(int velocity, double oldmin, double oldmax, double newmin,
		double newmax) {
	double oldrange = oldmax - oldmin;
	double newrange = newmax - newmin;
	double slope = newrange / oldrange;
	double zero = newmax - slope * oldmax;
	double output = slope * velocity + zero;
	int outint = int(output + 0.5);
	if (outint < 1) {
		outint = 1;
	}
	if (outint > 127) {
		outint = 127;
	}
	return outint;
}





