CXXFLAGS = -Wall -O2

all: prominence1d prominence

prominence1d: freader.o

prominence: LDFLAGS = -lboost_graph
ifdef NOGDAL
prominence: CXXFLAGS += -DNOGDAL
else
prominence: LDFLAGS += -lgdal
endif
prominence: greader.o
