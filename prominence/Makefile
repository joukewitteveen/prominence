CXXFLAGS = -Wall -O2
LDFLAGS = -lboost_graph
ifdef NOGDAL
CXXFLAGS += -DNOGDAL
else
LDFLAGS += -lgdal
endif

all = prominence minprominence
.PHONY: all
all: $(all)
$(all): greader.o preader.o
