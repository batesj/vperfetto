LDFLAGS=-static -O2 -DNDEBUG -Wl,--gc-sections
print-sync-times:
	$(CXX) $(LDFLAGS) -o $@ print-sync-times.cc get-sync-times.cc
