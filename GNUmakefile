# $Id: GNUmakefile,v 1.2 2004-06-01 07:05:03 gcosmo Exp $
# --------------------------------------------------------------
# GNUmakefile for LXe example.
# --------------------------------------------------------------

name := LXe
G4TARGET := $(name)
G4EXLIB := true

EXTRALIBS += -L$(CRYHOME)/lib -lCRY
CPPFLAGS  += -I$(CRYHOME)/src

export G4LIB_BUILD_STATIC=1
export G4LIB_BUILD_SHARED=

ifndef G4INSTALL
  #G4INSTALL = /nova/app/users/junting/geant4/geant4.9.6.p02-install/share/Geant4-9.6.2/geant4make/
  G4INSTALL = /Users/juntinghuang/geant4/geant4.9.6-install/share/Geant4-9.6.0/geant4make
endif

.PHONY: all
all: lib bin

include $(G4INSTALL)/config/architecture.gmk

include $(G4INSTALL)/config/binmake.gmk

LDFLAGS += $(shell $(ROOTSYS)/bin/root-config --glibs)
CPPFLAGS += $(shell $(ROOTSYS)/bin/root-config --cflags)
