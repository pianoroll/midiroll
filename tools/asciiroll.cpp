//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri Apr 13 13:30:54 PDT 2018
// Last Modified: Sat Apr 14 04:27:22 PDT 2018
// Filename:      midiroll/tools/asciiroll.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Convert a MIDI file into an ASCII format
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>

using namespace std;

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("x|hex|hexadecimal=b", "convert to hexadecimal byte codes");
	options.define("C|no-comments=b", "do not add comments about structure");
	options.define("b|binary=b", "output binary form to standard output");
	options.process(argc, argv);
	MidiRoll midiroll;
	if (options.getArgCount() == 0) {
		midiroll.read(cin);
	} else if (options.getArgCount() == 1) {
		midiroll.read(options.getArg(1));
	} else {
		cerr << "Too many arguments" << endl;
		exit(1);
	}
	if (options.getBoolean("hexadecimal")) {
		midiroll.writeHex(cout);
	} else if (options.getBoolean("no-comments")) {
		midiroll.writeBinasc(cout);
	} else if (options.getBoolean("binary")) {
		midiroll.write(cout);
	} else {
		cout << midiroll;
	}
	return 0;
}


