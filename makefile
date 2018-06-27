# Define default flags (include your source tree for example
INCFLAGS = -I./ -I/usr/include/wx-3.1-unofficial
CXXFLAGS = `wx-config --cxxflags`
LDFLAGS  = `wx-config --ldflags`
LDLIBS   = `wx-config --libs` -lmpir
REZFLAGS = `wx-config --rez-flags`

ifdef FINAL
	EXTRAFLAGS = -MD -O2 -fno-rtti -fno-exceptions -fomit-frame-pointer
else
	EXTRAFLAGS = -MD -g
endif

SOURCE_DIR = ./Mandelbrot\ Explorer/

SOURCES = main.cpp
SOURCES += MandelbrotExplorerWindow.cpp
SOURCES += MandelControlPanel.cpp
SOURCES += MandelView.cpp
SOURCES += ColorControlPanel.cpp
SOURCES += CustomResolutionDialog.cpp
SOURCES += MandelFractal.cpp
SOURCES += ColorDisplayPanel.cpp
SOURCES += ColorChannelGraph.cpp
SOURCES += ColorChannelCtrl.cpp


OBJECTS = $(addprefix $(SOURCE_DIR),$(SOURCES))

all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o mandelexplore $(OBJECTS) $(LDLIBS) $(INCFLAGS) -std=c++17

