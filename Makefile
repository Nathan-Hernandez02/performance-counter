all: performance-counter

performance-counter: performance-counter.c
	icc -o performance-counter performance-counter.c -lpapi -O3 -fp-model precise

clean:
	rm -f performance-counter
