//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat Apr 14 21:20:30 PDT 2018
// Last Modified: Sat Apr 14 21:32:18 PDT 2018
// Filename:      midiroll/tools/trackerize.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Convert from a scanned roll MIDI file into a
//                tracker-bar performed MIDI file.
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>

using namespace std;

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("h|tracker-height-in-pixels=i:25", "Height of tracker bar holes in pixels");
	options.define("o|output-file=s", "filename to save output results");
	options.process(argc, argv);
	MidiRoll rollfile;
	if (options.getArgCount() == 0) {
		rollfile.read(cin);
	} else if (options.getArgCount() == 1) {
		rollfile.read(options.getArg(1));
	} else {
		cerr << "Usage: " << options.getCommand() << "  [midifile]" << endl;
		exit(1);
	}
	rollfile.trackerize(options.getDouble("tracker-height-in-pixels"));
	string filename = options.getString("output-file");
	if ((!filename.empty()) && (options.getBoolean("output-file"))) {
		rollfile.write(filename);
	} else {
		cout << rollfile;
	}
	return 0;
}



