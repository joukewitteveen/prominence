CXXFLAGS = -Wall -O2 -DBACKEND_$(backend)

backend = gdal
ifeq ($(backend),boost)
LDFLAGS += -lboost_graph
endif
ifeq ($(backend),gdal)
LDFLAGS += -lgdal
endif

all = prominence minprominence
.PHONY: all
all: $(all)
$(all): preader.o backend/$(backend).o
