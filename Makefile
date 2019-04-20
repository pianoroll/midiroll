##
## Main GNU makefile for midiroll library.
##
## Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
## Creation Date: Wed Apr 11 16:33:22 PDT 2018
## Last Modified: Wed Apr 11 16:33:25 PDT 2018
## Filename:      midifile/Makefile
## Website:       http://github.com/craigsapp/midiroll
## Syntax:        GNU Makefile
## Description:   This Makefile can create the Midifile library and/or
##                example programs which use the Midifile library.
##
## To run this makefile, type:
##     make library
## then:
##     make programs
## Or type:
##     make
## to compile both the library and the programs at the same time.
##



##############################
##
## Targets:
##

# targets which don't actually refer to files
.PHONY : all info library examples programs bin options clean lib


all: info library programs lib


info:
	@echo ""
	@echo This makefile will compile the Midifile library and/or
	@echo the midiroll programs.  You may have to make the library
	@echo first if compiling the programs. Type one of the following:
	@echo "   make library"
	@echo or
	@echo "   make programs"
	@echo ""
	@echo To compile a specific program called xxx, type:
	@echo "   make xxx"
	@echo ""
	@echo Typing \"make\" alone will compile both the library and all programs.
	@echo ""


lib: library
library:
	$(MAKE) -f Makefile.library


clean:
	$(MAKE) -f Makefile.library clean
	-rm -rf lib

superclean: clean
	-rm -rf bin

update:
	(cd external; ./.download)

bin:      programs
examples: programs
programs:
	$(MAKE) -f Makefile.programs

install:
	sudo cp bin/* /usr/local/bin


##############################
##
## Default target: compile a particular program:
##

%:
	@-mkdir -p bin
	@echo compiling file $@
	$(MAKE) -f Makefile.programs $@


##############################
##
## Maintenance functions
##

update: midifile-update
midifile-update:
	(cd external && ./.download)


