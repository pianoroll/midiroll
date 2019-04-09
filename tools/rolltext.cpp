//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri Apr 13 13:30:54 PDT 2018
// Last Modified: Fri Mar 29 19:40:34 EDT 2019
// Filename:      midiroll/tools/rolltext.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Read/write/change text meta messages in MIDI roll files.
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>

using namespace std;
using namespace smf;

// function declarations:
void    processMidiFile    (MidiRoll& rollfile, Options& options, int index = -1);
void    queryParameter     (MidiRoll& rollfile, const string& query, bool fileQ);
void    setMetadata        (MidiRoll& rollfile, const string& key,
                            const string& value, const string& outputfile);
void    listAllText        (MidiRoll& rollfile, bool showTicks);
void    listAllMetadata    (MidiRoll& rollfile, bool showTicks);
void    errorMessage       (const string& message);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("k|key=s", "metadata key");
	options.define("f|filename=b", "display filename");
	options.define("v|value=s", "metadata value");
	options.define("o|output=s", "output file for writing/changing metadata");
	options.define("m|metadata=b", "list all metadata events in file");
	options.define("t|tick|ticks=b", "display tick");
	options.define("p|prefix=s:@", "metadata prefix character(s)");
	options.define("replace=b", "overwrite the input data with the output data");
	options.process(argc, argv);
	MidiRoll midiroll;
	if (options.getArgCount() == 0) {
		midiroll.read(cin);
		processMidiFile(midiroll, options);
	} else {
		if ((options.getArgCount() > 1) && options.getBoolean("output")) {
			errorMessage("Cannot write multiple input files to a single output file");
		}
		for (int i=0; i<options.getArgCount(); i++) {
			midiroll.read(options.getArg(i+1));
			processMidiFile(midiroll, options, i);
		}
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////

//////////////////////////////
//
// processMidiFile --
//

void processMidiFile(MidiRoll& rollfile, Options& options, int index) {
	if (options.getBoolean("prefix")) {
		rollfile.setMetadataMarker(options.getString("prefix"));
	}
	if (options.getBoolean("key") && options.getBoolean("value")) {
      // Need to set the given key to the value and then write the result to a file
      // or standard output.
		rollfile.setMetadata(options.getString("key"), options.getString("value"));
		if (options.getBoolean("output")) {
			// write to the given output file (should be guaranteed to be 
			// a single input by the calling function)
			rollfile.write(options.getString("output"));
		} else if ((index >= 0) && options.getBoolean("replace")) {
			// write to the same file that was read
			rollfile.write(options.getArg(index+1));
		} else {
			// write to standard outupt
			cout << rollfile;
		}
	} else if (options.getBoolean("key")) {
		// Searching for a given key, so print its value if present in the MIDI file:
		queryParameter(rollfile, options.getString("key"), options.getBoolean("filename"));
	} else if (options.getBoolean("metadata")) {
		listAllMetadata(rollfile, options.getBoolean("ticks"));
	} else {
		listAllText(rollfile, options.getBoolean("ticks"));
	}
}



//////////////////////////////
//
// queryParameter -- Return the metadata value for a given key paramter.
//      metadata key/value pairs are in the form:
//          @KEY: value
//      Will not print anything if the metadata key is not found.
//
/* test data:

"MThd"
4'6
2'0
2'1
2'600

"MTrk"
4'52
v0 FF 01 v14 "@KEY1: value 1"
v0 FF 01 v14 "@KEY2: value 2"
v0 FF 2F 00

 */
// Try:
//   cat test.txt | rolltext -q KEY2
// which should return:
//   value 2


void queryParameter(MidiRoll& rollfile, const string& key, bool fileQ) {
	string output;
	if (key.find(",") != string::npos) {
		stringstream fields(key);
		string field;
		while (getline(fields, field, ',')) {
			if (!output.empty()) {
				output += "\t";
			}
			output += rollfile.getMetadata(field);
		}
	} else {
		output = rollfile.getMetadata(key);
	}
	if (!output.empty()) {
		if (fileQ) {
			cout << rollfile.getFilename() << "\t";
		}
		cout << output << endl;
	}
}



//////////////////////////////
//
// setMetadata -- Add or change a metadata key/value pair.
//

void setMetadata(MidiRoll& rollfile, const string& key, const string& value,
		const string& outputfile) {
	if (outputfile.empty()) {
		cout << rollfile;
	} else {
		rollfile.write(outputfile);
	}
}



//////////////////////////////
//
// listAllText -- Print all text
//

void listAllText(MidiRoll& rollfile, bool showTicks) {
	vector<MidiEvent*> mes = rollfile.getTextEvents();
	for (int i=0; i<(int)mes.size(); i++) {
		if (showTicks) {
			cout << mes[i]->tick << "\t";
		}
		cout << mes[i]->getMetaContent() << endl;
	}
}



//////////////////////////////
//
// listAllMetadata -- Print all metadata events
//

void listAllMetadata(MidiRoll& rollfile, bool showTicks) {
	vector<MidiEvent*> mes = rollfile.getMetadataEvents();
	for (int i=0; i<(int)mes.size(); i++) {
		if (showTicks) {
			cout << mes[i]->tick << "\t";
		}
		cout << mes[i]->getMetaContent() << endl;
	}
}



//////////////////////////////
//
// errorMessage --
//

void errorMessage(const string& message) {
	cerr << message << endl;
	exit(1);
}



