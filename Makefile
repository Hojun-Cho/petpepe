pepe: pepe.c img.h
	cc -Wall -Wpedantic -o pepe pepe.c -lX11 -lXpm -lXext

clean:
	rm pepe
