CCFLAGS = -Wall -g -lm

all: compressT_LOLS compressR_LOLS compressR-worker_LOLS

compressT_LOLS: compressT_LOLS.c
	gcc $(CCFLAGS) -lpthread -o compressT_LOLS compressT_LOLS.c

compressR_LOLS: compressR_LOLS.c
	gcc $(CCFLAGS) -o compressR_LOLS compressR_LOLS.c

compressR-worker_LOLS: compressR-worker_LOLS.c
	gcc $(CCFLAGS) -o compressR-worker_LOLS compressR-worker_LOLS.c

clean:
	rm -rf compressT_LOLS compressR_LOLS compressR-worker_LOLS

