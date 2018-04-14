//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri Apr 13 06:56:36 PDT 2018
// Last Modified: Fri Apr 13 07:16:41 PDT 2018
// Filename:      midiroll/include/MidiRoll.h
// Syntax:        C++11
// vim:           ts=3 expandtab
//
// Description:   A class for manipulating piano rolls in MIDI file format.
//

#ifndef _MIDIROLL_H_INCLUDED
#define _MIDIROLL_H_INCLUDED

#include "MidiFile.h"

using namespace std;

class MidiRoll : public MidiFile {
	public:
		          MidiRoll   (void);
		          MidiRoll   (const char* aFile);
		          MidiRoll   (const string& aFile);
		          MidiRoll   (istream& input);
		          MidiRoll   (const MidiRoll& other);
		          MidiRoll   (MidiRoll&& other);
		         ~MidiRoll   ();

		MidiRoll& operator=  (const MidiRoll& other);
		MidiRoll& operator=  (const MidiFile& other);

		void                 setRollTempo       (double tempo,
                                               double dpi = 300.0);
		double               getRollTempo       (double dpi = 300.0);
      vector<MidiEvent*>   getTextEvents      (void);
      vector<MidiEvent*>   getMetadataEvents  (void);

      string               getMetadata        (const string& key);
      int                  setMetadata        (const string& key, 
                                               const string& value);


      // variable accessor functions:
      double               getLengthDpi       (void);
      void                 setLengthDpi       (double value);
      double               getWidthDpi        (void);
      void                 setWidthDpi        (double value);
      string               getMetadataMarker  (void);
      void                 setMetadataMarker  (const string& value);

   private:
      double m_lengthdpi      = 300.0;
      double m_widthdpi       = 300.0;
      string m_metadatamarker = "@";
};


#endif /* _MIDIROLL_H_INCLUDED */



