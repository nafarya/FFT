.PHONY: clean

all: fft

clean:
	rm -f fft

fft: fft.cpp
	$(CXX) -std=c++11 -O3 -Iinstall/include -Linstall/lib -lfftw3 fft.cpp -o fft