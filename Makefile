CXX=clang++
CPPFLAGS:= -std=c++14 -O0 -g -fno-omit-frame-pointer -fPIE

ifdef ASAN
CPPFLAGS += -fsanitize=address
endif

ifdef UB
CPPFLAGS += -fsanitize=undefined
endif

all: asan_go_boom.x asan_go_boom.s ubsan_go_boom.x

asan_go_boom.x: asan_go_boom.o
	$(CXX) $(CPPFLAGS) -o $@ $<

asan_go_boom.o: asan_go_boom.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@

ubsan_go_boom.x: ubsan_go_boom.o
	$(CXX) $(CPPFLAGS) -o $@ $<

ubsan_go_boom.o: ubsan_go_boom.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@


asan_go_boom.s: asan_go_boom.cpp
	$(CXX) $(CPPFLAGS) -S -masm=intel $<

clean:
	rm -f *.x *.s *.o
