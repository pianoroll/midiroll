//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sun Aug 29 13:33:39 CEST 2021
// Last Modified: Sun Aug 29 14:13:07 CEST 2021
// Filename:      midiroll/tools/holegap.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Measure the distance between each hole for each
//                tracker-bar position on the roll.  The input should
//                be in the raw-midi format, such as:
//                   https://github.com/pianoroll/SUPRA/tree/master/welte-red/midi-raw
//
// Usage:         bin/holegap [-a #] [-m #] [-x #] file(s)
//
// Options:       -a # : average +/- pixels on each side of current pixel (3rd column)
//                -m # : minimum pixel distance to track
//                -x # : maximum pixel distance to track
//

#include "Options.h"
#include "MidiRoll.h"
#include <iostream>
#include <string>

using namespace std;
using namespace smf;

// function declarations:
void processMidiFile  (vector<int>& histogram, MidiRoll& rollfile);
void printResults     (vector<int>& histogram, int minlen, int average);

int minlen = 0;	// minimum pixel distance to track
int maxlen = 99;	// maximum pixel distance to track
int average = 6;  // number of pixels on each side of target pixel to average

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("a|avg|average=i:6", "Number of pixels to average * 2 + 1");
	options.define("m|min=i:0", "Minimum pixel size to track");
	options.define("x|max=i:99", "Maximum pixel size to track");
	options.process(argc, argv);

	minlen = options.getInteger("min");
	maxlen = options.getInteger("max");

	vector<int> histogram(maxlen - minlen + 1, 0);
	
	MidiRoll midiroll;
	if (options.getArgCount() == 0) {
		midiroll.read(cin);
		processMidiFile(histogram, midiroll);
	} else {
		for (int i=0; i<options.getArgCount(); i++) {
			midiroll.read(options.getArg(i+1));
			processMidiFile(histogram, midiroll);
		}
	}
	printResults(histogram, minlen, average);
	return 0;
}

///////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// processMidiFile --
//

void processMidiFile(vector<int>& histogram, MidiRoll& rollfile) {
	vector<int> lastOffTime;

	for (int i=0; i<rollfile.getTrackCount(); i++) {
		MidiEventList* mel = &rollfile[i];
		lastOffTime.resize(127);
		fill(lastOffTime.begin(), lastOffTime.end(), -1);
		for (int j=0; j<mel->getEventCount(); j++) {
			MidiEvent* me = &(*mel)[j];
			if (me->isNoteOff()) {
				int key = me->getKeyNumber();
				int tick = me->tick;
				lastOffTime.at(key) = tick;
			} else if (me->isNoteOn()) {
				int key = me->getKeyNumber();
				int tick = me->tick;
				if (lastOffTime.at(key) >= 0) {
					int difference = tick - lastOffTime.at(key);
					if ((difference >= minlen) && (difference <= maxlen)) {
						histogram[difference - minlen]++;
					}
				}
			}
		}
	}
}



//////////////////////////////
//
// printResults --
//

void printResults(vector<int>& histogram, int minlen, int average) {
	for (int i=0; i<(int)histogram.size(); i++) {
		cout << (i+minlen) << "\t" << histogram[i];
		if ((average > 0) && (i - average >= 0) && (i + average < (int)histogram.size())) {
			double sum = 0;
			for (int j = i - average; j <= i + average; j++) {
				sum += histogram.at(j);
			}
			double total = sum / (average * 2 + 1);
			total = (int(total * 10.0 + 0.5))/10.0;
			cout << "\t" << total;
		}
		cout << endl;
	}
}



