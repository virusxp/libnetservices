PROJECT=netservices
SOURCES=tcp/tcp_client.cpp
LIBRARY=shared
INCPATHS=.
LIBPATHS=
LDFLAGS=
CFLAGS=-c -Wall -Werror -std=c99
CXXFLAGS=-c -Wall -Werror -std=c++17
CC=gcc
CXX=g++

# ------------ MAGIC BEGINS HERE -------------

# Automatic generation of some important lists
OBJECTS=$(SOURCES:.cpp=.o)
INCFLAGS=$(foreach TMP,$(INCPATHS),-I$(TMP))
LIBFLAGS=$(foreach TMP,$(LIBPATHS),-L$(TMP))

# Set up the output file names for the different output types
ifeq "$(LIBRARY)" "shared"
    BINARY=lib$(PROJECT).so
    LDFLAGS += -shared
else ifeq "$(LIBRARY)" "static"
    BINARY=lib$(PROJECT).a
else
    BINARY=$(PROJECT)
endif

all: $(OBJECTS) $(BINARY)

$(BINARY): $(OBJECTS)
    # Link the object files, or archive into a static library
    ifeq "$(LIBRARY)" "static"
        ar rcs $(BINARY) $(OBJECTS)
    else
		$(CXX) $(LIBFLAGS) $(OBJECTS) $(LDFLAGS) -o $@
    endif

%.o: %.c
	$(CC) $(INCFLAGS) $(CFLAGS) -fPIC $< -o $@

%.o: %.cpp
	$(CXX) $(INCFLAGS) $(CXXFLAGS) -fPIC $< -o $@

distclean: clean
	rm -f $(BINARY)

clean:
	rm -f $(OBJECTS)