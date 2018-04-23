//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri Apr 13 06:56:36 PDT 2018
// Last Modified: Mon Apr 23 10:51:16 PDT 2018
// Filename:      midiroll/include/MidiRoll.h
// Syntax:        C++11
// vim:           ts=3 expandtab
//
// Description:   A class for manipulating piano rolls in MIDI file format.
//

#ifndef _MIDIROLL_H_INCLUDED
#define _MIDIROLL_H_INCLUDED

#include "MidiFile.h"

#include <string>
#include <vector>
#include <iostream>

namespace smf {

class MidiRoll : public MidiFile {
	public:
		                       MidiRoll   (void);
	             	           MidiRoll   (const char* aFile);
		                       MidiRoll   (const std::string& aFile);
		                       MidiRoll   (std::istream& input);
		                       MidiRoll   (const MidiRoll& other);
		                       MidiRoll   (MidiRoll&& other);
		                      ~MidiRoll   ();

		MidiRoll&               operator=  (const MidiRoll& other);
		MidiRoll&               operator=  (const MidiFile& other);

		void                    setRollTempo       (double tempo,
		                                            double dpi = 300.0);
		double                  getRollTempo       (double dpi = 300.0);
		std::vector<MidiEvent*> getTextEvents      (void);
		std::vector<MidiEvent*> getMetadataEvents  (void);

		std::string             getMetadata        (const std::string& key);
		int                     setMetadata        (const std::string& key,
		                                            const std::string& value);

		// tracker bar emulation:
		void                    trackerize         (int trakerheight);

		// acceleration emulation:
		void                    removeAcceleration (void);
		void                    applyAcceleration  (double inches,
		                                            double percent);
      // tick conversions:
		void                    convertToMillisecondTicks (void);

		// variable accessor functions:
		double                  getLengthDpi       (void);
		void                    setLengthDpi       (double value);
		double                  getWidthDpi        (void);
		void                    setWidthDpi        (double value);
		std::string             getMetadataMarker  (void);
		void                    setMetadataMarker  (const std::string& value);

	private:
		double m_lengthdpi           = 300.0;
		double m_widthdpi            = 300.0;
		std::string m_metadatamarker = "@";
};

} // end smf namespace

#endif /* _MIDIROLL_H_INCLUDED */



