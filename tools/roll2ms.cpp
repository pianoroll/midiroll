//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon Apr 16 10:38:44 PDT 2018
// Last Modified: Mon Apr 16 10:38:47 PDT 2018
// Filename:      midiroll/tools/roll2ms.cpp
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
	options.process(argc, argv);
	MidiRoll midiroll;
	if (options.getArgCount() == 0) {
		midiroll.read(cin);
	} else if (options.getArgCount() == 1) {
		midiroll.read(options.getArg(1));
	} else {
		cout << "Error: only one filename as input is allows." << endl;
		exit(1);
	}
	processMidiFile(midiroll, options);
	return 0;
}


///////////////////////////////////////////////////////////////////////////

//////////////////////////////
//
// processMidiFile --
//

void processMidiFile(MidiRoll& rollfile, Options& options) {
	rollfile.convertToMillisecondTicks();
	string filename = options.getString("output");
	if (!filename.empty()) {
		rollfile.write(filename);
	} else {
		cout << rollfile;
	}
}



