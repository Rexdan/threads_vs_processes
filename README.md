# Processes vs Threads

## What is it?
  * This program takes a given file by the user and compresses it with the LOLS text compression algorithm into different files. 

  * These files will all be appended with “_LOLS” followed by a number representing the requested number of partitions by the user should this number be more than 1. 

  * In addition, the user may choose between two versions of this program--one that implements the compression via threads and one that runs the compression via processes. 

## How do I use it?
1. Compile compressR_LOLS.c & compressR-worker_LOLS.c & compressT_LOLS.c with “make” command.

2. Run compressR_LOLS specifying the file to be compressed and number of files to be outputed `./compress(R/T)_LOLS [file to be compressed] [number of output]`

    -Example using processes: `./compressR_LOLS words.txt 2`
  
    -Example using threads: `./compressT_LOLS words.txt 5`

3. The desired output will be the compressed version of the original file with the first file containing the over flow words.txt: aaalbbb words.LOLS0: 3al wordsLOLS1: 3b.

## What makes it tick?
After checking the last argument from the command line that would indicate the number of partitions/threads, we use that number to determine the number of processes/threads that we will create. Each process/thread will be given its own process/thread number so that it may know whether or not it is the first, the last, or some process/thread in between. We take the number of bytes that the 
original file contains and logically compute the number of bytes that a given process/thread should read from the file. Given the fact that each thread/process will be accessing the original file in a predetermined segment and that each of these segments will be disjoint, we need not worry about synchronization. 

### The Compression Algorithm
The compression function cmprss takes a string argument which is part of the file that has been read. The algorithm then reads characters individually and checks for repeating alphabetical characters. If the number ofrepeating characters is larger than 2 then the string saved is, number corresponding to the number of repetitions concatenated with the repeating character itself. If a non-alphabetical character is detected an error message occurs, and the compression continues ignoring the illegal characters. 

#### Special Note
When compressing very large text files (e.g. 10mb), a good number of partitions would be 10% of the original size. It was said in lecture that we need not worry about abruptly terminating the program if we encounter a non-letter. Rather, we simply print a warning message. 
