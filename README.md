# MIDI Piano Rolls

A C++ library to process MIDI files extracted from piano-roll scan images.

## Downloading

To download using git in a terminal:

```bash
git clone https://github.com/craigsapp/midiroll
```

## Compiling

To compile, type:

```bash
make
```

This will compile the utility programs and place them in the `bin` directory.



## Tools


| Name                | Description                                        |
|---------------------|----------------------------------------------------|
| asciiroll           | Convert between ASCII and binary MIDI files        |
| countnotes          | Count the number of notes for each pitch.  Useful to verify roll format. |
| fped                | Convert sustain pedal into note extensions.        |
| mirror              | Mirror pitch to check for images which are scaned in mirrored orientation. |
| notelist            |                                                    |
| roll2ms             |                                                    |
| expscale.cpp        | Rescale note velocities to a new range.            |
| roll2mstick         | Convert tempo messages to millisecond tick values. |
| rollaccel           | Model roll acceleration.                           |
| rollbreak           |                                                    |
| rolltempo           |                                                    |
| rolltext            | Add/read metadata entries in a MIDI file.          |
| setinstrument       | Set the instruments used in the MIDI file.         |
| tempomm             |                                                    |
| tick2time           |                                                    |
| trackerize          | Model tracker bar hole extension.                  |

