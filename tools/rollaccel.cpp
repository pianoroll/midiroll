//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sun Apr 15 22:50:08 PDT 2018
// Last Modified: Sun Apr 15 22:50:11 PDT 2018
// Filename:      midiroll/tools/rollaccel.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Add/change/remove roll acceleration emulation.
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>
#include <string>

using namespace std;

// function declarations:
void    processMidiFile    (MidiRoll& rollfile, Options& options);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("o|output=s", "output file for writing/changing metadata");
	options.define("r|remove=b", "remove tempo acceleration emulation");
	options.define("s|step=d:12", "inches between each tempo increment step");
	options.define("p|percent=d:0.04", "percent acceleration at each step");
	options.process(argc, argv);
	MidiRoll midiroll;
	if (options.getArgCount() == 0) {
		midiroll.read(cin);
		processMidiFile(midiroll, options);
	} else {
		for (int i=0; i<options.getArgCount(); i++) {
			midiroll.read(options.getArg(i+1));
			processMidiFile(midiroll, options);
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
	double percent = options.getDouble("percent");
	double inches  = options.getDouble("step");

	if (options.getBoolean("remove")) {
		rollfile.removeAcceleration();
	} else if (percent > 0.0) {
		if (inches > 0.0) {
			rollfile.applyAcceleration(inches, percent);
		}
	}

	string filename = options.getString("output");
	if (!filename.empty()) {
		rollfile.write(filename);
	} else {
		cout << rollfile;
	}
}



