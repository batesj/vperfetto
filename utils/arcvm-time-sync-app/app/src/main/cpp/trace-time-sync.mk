CFLAGS=-I. -Wall -DNDEBUG
DEPS=time-trace.h perfetto.h
OBJ=get-sync-times.o time-trace.o trace-time-sync.o perfetto.o 

%.o: %.cc $(DEPS)
	ccache $(CXX) -c -o $@ $< $(CFLAGS)

trace-time-sync: $(OBJ)
	$(CXX) -static -o $@ $^ $(CFLAGS) -pthread