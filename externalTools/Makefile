include ../include.mk
.PHONY: all clean cutestM cutestM.clean quicktreeM.clean

libPath = ../lib

all : cutestM quicktreeM

clean : cutestM.clean quicktreeM.clean

cutestM :
	cd cutest && make all

cutestM.clean :
	cd cutest && make clean

quicktreeM :
	cd quicktree_1.1 && make all

quicktreeM.clean :
	cd quicktree_1.1 && make clean
