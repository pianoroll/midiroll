//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri Apr 13 06:56:36 PDT 2018
// Last Modified: Fri Apr 13 08:11:17 PDT 2018
// Filename:      midiroll/src/MidiRoll.cpp
// Syntax:        C++11
// vim:           ts=3 expandtab
//
// Description:   A class for manipulating piano rolls in MIDI file format.
//


#include "MidiRoll.h"

#include <iostream>

using namespace std;


//////////////////////////////
//
// MidiRoll::MidiRoll -- Class constructors.
//

MidiRoll::MidiRoll(void) : MidiFile() { }
MidiRoll::MidiRoll(const char* aFile) : MidiFile(aFile) { }
MidiRoll::MidiRoll(const string& aFile) : MidiFile(aFile) { }
MidiRoll::MidiRoll(istream& input) : MidiFile(input) { }
MidiRoll::MidiRoll(const MidiRoll& other) : MidiFile(other) { }
MidiRoll::MidiRoll(MidiRoll&& other) : MidiFile(other) { }



//////////////////////////////
//
// MidiRoll::MidiRoll -- Class deconstructor.
//

MidiRoll::~MidiRoll() { }



//////////////////////////////
//
// MidiRoll::operator= -- Copy constructor.
//

MidiRoll& MidiRoll::operator=(const MidiRoll& other) { 
	if (this == &other) {
		return *this;
	}
	MidiFile::operator=(other);
	return *this;
}

MidiRoll& MidiRoll::operator=(const MidiFile& other) { 
	MidiFile::operator=(other);
	return *this;
}



//////////////////////////////
//
// MidiRoll::setRollTempo -- Set the piano-roll tempo of the MIDI file.
//      The tempo is controlled by the ticks-per-quarter-note value in
//      the MIDI header rather than by a tempo meta message.  The tempo
//      meta messages are instead used to control an emulation of the
//      roll acceleration over time.  The final TPQ value will be rounded
//      to the nearest integer.
//
// default value: dpi = 300.0
// The default dpi of the rolls is set to 300 since this is the scanning
// resolution of the Stanford piano rolls (which are more precisely scanned
// at 300.25 dpi).  Each tick represents one image pixel row.
// A tempo of 100 means the roll moves (at its start) by
// 10.0 feet per minute.  This is 10.0 * 300 * 12 = 36000 rows/minute.
// A reference tempo of 60 bpm is used at the start of the acceleration,
// so each "quarter note" is 36000 / 60 = 600 rows.

void MidiRoll::setRollTempo(double tempo, double dpi) {
	int tpq = int(tempo/10.0 * dpi*12.0/60.0 + 0.5);
	if (tpq < 1) {
      // SMPTE ticks or error: do not alter
		cerr << "Error: tpq is too small: " << tpq << endl;
		return;
	}
	if (tpq > 0x7FFF) {
      // SMPTE ticks: do not alter
		cerr << "Error: tpq is too large: " << tpq << endl;
		return;
	}
	MidiFile::setTicksPerQuarterNote(tpq);
}



//////////////////////////////
//
// MidiRoll::getRollTempo -- Return the piano-roll tempo.
// default value: dpi = 300.0
//

double MidiRoll::getRollTempo(double dpi) {
	int tpq = MidiFile::getTicksPerQuarterNote();
	double tempo = tpq * 10.0 / dpi / 12.0 * 60.0;
	return tempo;
}



