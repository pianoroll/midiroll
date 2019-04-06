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
#include <vector>
#include <regex>
#include <string>


namespace smf {

//////////////////////////////
//
// MidiRoll::MidiRoll -- Class constructors.
//

MidiRoll::MidiRoll(void) : MidiFile() { }
MidiRoll::MidiRoll(const char* aFile) : MidiFile(aFile) { }
MidiRoll::MidiRoll(const std::string& aFile) : MidiFile(aFile) { }
MidiRoll::MidiRoll(std::istream& input) : MidiFile(input) { }
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
		std::cerr << "Error: tpq is too small: " << tpq << std::endl;
		return;
	}
	if (tpq > 0x7FFF) {
		// SMPTE ticks: do not alter
		std::cerr << "Error: tpq is too large: " << tpq << std::endl;
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



//////////////////////////////
//
// MidiRoll::getTextEvents -- Return a list of all MIDI events which are
//    meta-message text events.
//

std::vector<MidiEvent*> MidiRoll::getTextEvents(void) {
	std::vector<MidiEvent*> mes;
	for (int i=0; i<getTrackCount(); i++) {
		for (int j=0; j<operator[](i).getSize(); j++) {
			MidiEvent* mm = &operator[](i)[j];
			if (!mm->isMetaMessage()) {
				continue;
			}
			int mtype = mm->getMetaType();
			if (mtype != 0x01) {
				continue;
			}
			mes.push_back(mm);
		}
	}
	return mes;
}



//////////////////////////////
//
// MidiRoll::getMetadataEvents -- Return a list of all MIDI events
//    that are meta-message text events that have the structure of
//    a metadata key/value pair.
//

std::vector<MidiEvent*> MidiRoll::getMetadataEvents(void) {
	std::vector<MidiEvent*> mes;
	std::string marker = getMetadataMarker();
	for (int i=0; i<getTrackCount(); i++) {
		for (int j=0; j<operator[](i).getSize(); j++) {
			MidiEvent* mm = &operator[](i)[j];
			if (!mm->isMetaMessage()) {
				continue;
			}
			int mtype = mm->getMetaType();
			if (mtype != 0x01) {
				continue;
			}
			std::string content = mm->getMetaContent();
			if (content.compare(0, marker.size(), marker) != 0) {
				continue;
			}
			if (content.find(":", marker.size()) == std::string::npos) {
				continue;
			}
			mes.push_back(mm);
		}
	}
	return mes;
}



//////////////////////////////
//
// MidiRoll::metadata -- returns the value associated with a metadata key.
//     returns an empty string if the metadata key is not found.  The returned
//     value has whitespace trimmed from front and back of value.  Metadata will
//     only be searched for in the first track of the file.  Only the first
//     occurrence of the metadata key will be considered.
//

std::string MidiRoll::getMetadata(const std::string& key) {
	std::string output;
	std::string query;
	query += getMetadataMarker();
	query += key;
	query += ":\\s*(.*)\\s*$";
	std::regex re(query);
	std::smatch match;
	MidiRoll& mr = *this;
	for (int i=0; i<mr[0].size(); i++) {
		if (!mr[0][i].isText()) {
			continue;
		}
		std::string content = mr[0][i].getMetaContent();
		try {
			if (std::regex_search(content, match, re) && (match.size() > 1)) {
				output = match.str(1);
				break;
			}
		} catch (std::regex_error& e) {
			std::cerr << "PROBLEM SEARCHING FOR METADATA" << std::endl;
		}
	}
	return output;
}



//////////////////////////////
//
// MidiRoll::setMetadata -- Change the value of a given metadata key.
//    If there is no key for that metadata value, the add it.
//

int MidiRoll::setMetadata(const std::string& key, const std::string& value) {
	if (key.empty()) {
		std::cerr << "KEY CANNOT BE EMPTY" << std::endl;
		return -1;
	}
	bool found = false;
	int output = 0;
	std::string query;
	query += getMetadataMarker();
	query += key;
	query += ":\\s*(.*)\\s*$";
	std::regex re(query);
	std::smatch match;
	MidiRoll& mr = *this;
	for (int i=0; i<mr[0].size(); i++) {
		if (!mr[0][i].isText()) {
			continue;
		}
		std::string content = mr[0][i].getMetaContent();
		try {
			if (std::regex_search(content, match, re) && (match.size() > 1)) {
				std::string newline;
				newline += getMetadataMarker();
				newline += key;
				newline += ":";
            if ((!value.empty()) && (!isspace(value[0]))) {
            } else {
               newline += " ";
            }
				newline += value;
				mr[0][i].setMetaContent(newline);
				found = true;
				output = mr[0][i].tick;
				break;
			}
		} catch (std::regex_error& e) {
			std::cerr << "PROBLEM SEARCHING FOR METADATA" << std::endl;
		}
	}
	if (found) {
		return output;
	}

	std::string newline;
	newline += getMetadataMarker();
	newline += key;
	newline += ": ";
	newline += value;

	mr.addText(0, 0, newline);
	mr.sortTrack(0);

	return output;
}



//////////////////////////////
//
// MidiRoll::trackerize -- Emulate tracker bar extension
//     of holes on scan.
//

void MidiRoll::trackerize(int trackerheight) {
	MidiRoll& mr = *this;

	mr.joinTracks();   // make a single list of events
	mr.linkNotePairs();

	for (int i=0; i<mr[0].getSize(); i++) {
		if (!mr[0][i].isNoteOn()) {
			continue;
		}
		MidiEvent* me = mr[0][i].getLinkedEvent();
		if (!me) {
			std::cerr << "MISSING NOTE OFF" << std::endl;
			continue;
		}
		me->tick += trackerheight;
	}

	mr.splitTracks(); // split events into separate tracks again
	mr.sortTracks();  // necessary since timestamps have been changed
}



//////////////////////////////
//
// MidiRoll::removeAcceleration -- Remove any tempo meta messages
//    which are intended to emulate roll acceleration on the pickup
//    spool of a player piano.
//

void MidiRoll::removeAcceleration (void) {
	MidiRoll& mr = *this;
	for (int i=0; i<mr[0].size(); i++) {
		if (!mr[0][i].isTempo()) {
			continue;
		}
		mr[0][i].clear();
	}
	// Need to add tempo = 60 at tick 0
	MidiFile::addTempo(0, 0, 60.0);
	MidiFile::sortTrack(0);
}



//////////////////////////////
//
// MidiRoll::applyAcceleration -- Emulate roll acceleration according
//    to the input paramters.
// default values:
//      inches  = 12.0;
//      percent = 0.04;
//

void MidiRoll::applyAcceleration(double inches, double percent) {
	removeAcceleration();  // adds first tempo at 60.0
	double factor  = 1.0 + percent / 100.0;
	int    maxtick = getMaxTick();
	double step    = getLengthDpi() * inches;
	int    count   = int(maxtick / step);
	double tempo   = 60.0 * factor;
	for (int i=1; i<count; i++) {
		addTempo(0, (int)(i*step+0.5), tempo);
		tempo *= factor;
	}
	sortTrack(0);
}



//////////////////////////////
//
// MidiRoll::convertToMillisecondTicks -- Convert from ticks representing
//     image pixel rows into ticks representing milliseconds.  MIDI file
//     is assumed to be in absolute tick mode before calling this function.
//

void MidiRoll::convertToMillisecondTicks(void) {
   doTimeAnalysis();
   setMillisecondTicks();
   MidiRoll& mr = *this;
   for (int i=0; i<mr.getTrackCount(); i++) {
      for (int j=0; j<mr[i].getEventCount(); j++) {
         mr[i][j].tick = int(mr[i][j].seconds * 1000.0 + 0.5);
      }
   }
	removeAcceleration();
}



//////////////////////////////
//
// MidiRoll::getLengthDpi -- Get the DPI resolution of the original scan
//    along the length of the piano roll.
//

double MidiRoll::getLengthDpi(void) {
	return m_lengthdpi;
}



//////////////////////////////
//
// MidiRoll::setLengthDpi -- Set the DPI resolution of the original scan
//     along the length of the piano roll.
//

void MidiRoll::setLengthDpi(double value) {
	if (value > 0) {
		m_lengthdpi = value;
	}
}



//////////////////////////////
//
// MidiRoll::getWidthDpi -- Get the DPI resolution of the original
//    scan across the width of the piano roll.
//

double MidiRoll::getWidthDpi(void) {
	return m_widthdpi;
}



//////////////////////////////
//
// MidiRoll::setWidthDpi -- Set the DPI resolution of the original scan
//     across the width of the piano roll.
//

void MidiRoll::setWidthDpi(double value) {
	if (value > 0) {
		m_widthdpi = value;
	}
}



//////////////////////////////
//
// MidiRoll::getMetadataMarker --
//

std::string MidiRoll::getMetadataMarker(void) {
	return m_metadatamarker;
}



//////////////////////////////
//
// MidiRoll::getMetadataMarker --
//

void MidiRoll::setMetadataMarker(const std::string& value) {
	m_metadatamarker = value;
}


} // end smf namespace



