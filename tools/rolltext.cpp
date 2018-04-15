//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri Apr 13 13:30:54 PDT 2018
// Last Modified: Sat Apr 14 04:27:22 PDT 2018
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

using namespace std;

// function declarations:
void    processMidiFile    (MidiRoll& rollfile, Options& options);
void    queryParameter     (MidiRoll& rollfile, const string& query);
void    setMetadata        (MidiRoll& rollfile, const string& key,
                            const string& value, const string& outputfile);
void    listAllText        (MidiRoll& rollfile, bool showTicks);
void    listAllMetadata    (MidiRoll& rollfile, bool showTicks);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("k|key=s", "metadata key");
	options.define("v|value=s", "metadata value");
	options.define("o|output=s", "output file for writing/changing metadata");
	options.define("m|metadata=b", "list all metadata events in file");
	options.define("t|tick|ticks=b", "display tick");
	options.define("p|prefix=s", "metadata prefix charcter(s)");
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
	if (options.getBoolean("prefix")) {
		rollfile.setMetadataMarker(options.getString("prefix"));
	}
	if (options.getBoolean("key") && options.getBoolean("value")) {
		setMetadata(rollfile, options.getString("key"), options.getString("value"),
				options.getString("output"));
	} else if (options.getBoolean("key")) {
		queryParameter(rollfile, options.getString("key"));
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


void queryParameter(MidiRoll& rollfile, const string& key) {
	string output = rollfile.getMetadata(key);
	if (!output.empty()) {
		cout << output << endl;
	}
}



//////////////////////////////
//
// setMetadata -- Add or change a metadata key/value pair.
//

void setMetadata(MidiRoll& rollfile, const string& key, const string& value,
		const string& outputfile) {
	rollfile.setMetadata(key, value);
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



