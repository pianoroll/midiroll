//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri Feb  7 13:39:22 PST 2020
// Last Modified: Fri Feb  7 13:39:24 PST 2020
// Filename:      midiroll/tools/copyaccel.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Copy the acceleration model from one MIDI file to another.
//                The acceleration is stored as tempo changes in the first
//                channel.  In the file to which the acceleration is copied,
//                tempo changes are removed.
//
// Options:
//                -f input.mid   == the file to copy acceleration emulation from (required).
//                -t output.mid  == the file to copy acceleration emulation to (required).
//                -r             == replace the contents of output with the new acceleration (optional).
//                -l             == list the current acceleration emulation in input and output files (optional).
//                -d             == delete acceleration tempo messages.
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>
#include <string>

using namespace std;
using namespace smf;

// function declarations:
void processMidiFile   (MidiFile& output, MidiFile& input);
void printTempoList    (MidiFile& file);
void deleteTempo       (MidiFile& file);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("f|from=s", "Source MIDI file for copying acceleration");
	options.define("t|to=s",   "Target MIDI file for copying acceleration");
	options.define("r|replace|o|overwrite=b",   "Replace contents of output file with new timings");
	options.define("l|list=b",   "List the tempo messages for acceleration for file(s)");
	options.define("d|delete=b",  "Delete tempo messages for acceleration for file(s)");

	options.process(argc, argv);
	if (options.getArgCount() != 0) {
		cerr << "Usage: " << options.getCommand() << " -f input.mid -t output.mid" << endl;
		exit(1);
	}

	string inname  = options.getString("from");
	string outname = options.getString("to");

	MidiFile input;
	MidiFile output;

	if (!inname.empty()) {
		input.read(inname);
	}
	if (!outname.empty()) {
		output.read(outname);
	}

	if (options.getBoolean("delete") && !outname.empty()) {
		deleteTempo(output);
		if (options.getBoolean("replace")) {
			output.write(outname);
		} else {
			cout << output << endl;
		}
		return 0;
	}

	if (options.getBoolean("list") && !inname.empty()) {
		cout << "!! Current input file tempos" << endl;
		printTempoList(input);
	}
	if (options.getBoolean("list") && !outname.empty()) {
		cout << "!! Current output file tempos" << endl;
		printTempoList(output);
	}
	if (options.getBoolean("list")) {
		return 0;
	}

	if (inname.empty()) {
		cerr << "Requires an input file name using -f option" << endl;
		cerr << "Usage: " << options.getCommand() << " -f input.mid -t output.mid" << endl;
		exit(1);
	}
	if (outname.empty()) {
		cerr << "Requires an output file name using -t option" << endl;
		cerr << "Usage: " << options.getCommand() << " -f input.mid -t output.mid" << endl;
		exit(1);
	}

	processMidiFile(output, input);

	if (options.getBoolean("replace")) {
		output.write(outname);
	} else {
		cout << output << endl;

		for (int i=0; i<output[0].getEventCount(); i++) {
			if (!output[0][i].isTempo()) {
				continue;
			}
		}
	}

	return 0;
}



//////////////////////////////
//
// printTempoList -- Print tempos from first track.
//

void printTempoList(MidiFile& file) {
	cout << "**tick\t**usec\t**tempo\n";
	for (int i=0; i<file[0].getEventCount(); i++) {
		if (!file[0][i].isTempo()) {
			continue;
		}
		int    microseconds = file[0][i].getTempoMicroseconds();
		double tempo        = file[0][i].getTempoBPM();
		int    tick         = file[0][i].tick;
		cout << tick << "\t" << microseconds << "\t" << tempo << endl;
	}
	cout << "*-\t*-\t*-\n";

}



//////////////////////////////
//
// processMidiFile --
//

void processMidiFile(MidiFile& output, MidiFile& input) {

	// clear existing tempo messages in output file, first track;
	deleteTempo(output);

	// then copy new tempo messages from input file's first track:
	for (int i=0; i<input[0].getEventCount(); i++) {
		if (!input[0][i].isTempo()) {
			continue;
		}
		output[0].push_back(input[0][i]);
	}
	output.sortTracks();
}



//////////////////////////////
//
// deleteTempo --
//

void deleteTempo(MidiFile& file) {
	for (int i=0; i<file[0].getEventCount(); i++) {
		if (file[0][i].isTempo()) {
			file[0][i].clear();
		}
	}
	file.removeEmpties();
}





