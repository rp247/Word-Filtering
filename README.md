Ruchit Patel

---------------------
DESCRIPTION

- A data filtering program is implemented. 
- The data is parsed using regular expressions and filtered with Bloom filter according to words added in a Hash Table.
- The lab produces one executable: Banhammer (banhammer to be exact).
- Command line arguments:   -h (prints help message), 
		            -t (specifies the size of the hash table), 
		            -f (specifies the size of the bloom filter), 
			    -s (only print the statistics),
			    -m (use the move-to-front rule).

---------------------
DIFFERENCES

- The graphs including statistics for LinkedLists in my writeup would differ than the actual statistics for my executable. This is because my current executable was quite slow in producing the graphs and so I had to make a faster version to make the graphs. The executable used to make those graphs cane be build from this commit ID: b3430aa4fa754e6311e5b971bb0ef76f6ac3e207. If I make graphs including LL from my current source files, the graphs in my wirteup concerning LL would be simply scaled up by some factor and hence there would no major/practical difference. The analysis provided in the writeup will still hold true. 

---------------------
FILES

1. banhammer.h
- THIS FILE HAS BEEN DELETED FROM THE REPO. IGNORE THIS ENTRY.

2. banhammer.c
- This source file contains the main method and the implementation for the logic for this lab. 

3. messages.h
- This header file contains the messages to be given by the program after filtering the words in the main method.

4. speck.h
- This header file contains the declaration of the hash method made in speck.c.

5. speck.c
- This source file contains the implementation of the speck cipher and the hashing function used throughout this program (provided in the folder).

6. ht.h
- This header file declares the Hash Table abstract data structure and the methods to manipulate it.

7. ht.c
- This source file implements the methods declared in ht.h to work with a Hash Table.

8. ll.h
- This header file declares the LinkedList abstract data structure and the methods to manipulate it.

9. ll.c
- This source file implements the methods declared in ll.h to work with a LinkedList.

10. node.h
- This header file declares and defines the Node abstract data structure and the methods to manipulate it.

11. node.c
- This source file implements the methods declared in node.h to work with a Node.

12. bf.h
- This header file declares the Bloom Filter abstract data structure and the methods to manipulate it.

13. bf.c
- This source file implements the methods declared in bf.h to work with a Bloom Filter.

14. bv.h
- This header file declares the BitVector abstract data structure and the methods to manipulate it.

15. bv.c
- This source file implements the methods declared in bv.h to work with a BitVector.

16. parser.h
- This header file declares the methods associated with the parsing module (it is the interface to it).

17. parser.c
- This source file implements the regex parsing module (provided for the lab).

18. Makefile

- This is a Makefile that can be used with the make utility to build the executables.

19. DESIGN.pdf 

- This PDF explains the design for this lab. It includes a brief description of the lab and pseudocode alongwith implementation description. 

20. WRITEUP.pdf

- This file contains the writeup for the lab which has several graphs such as change in load, size, etc. and it also contains analysis of those graphs.

---------------------
INSTRUCTIONS

With make:
1. Keep the Makefile in the same directory as all other files. 

2. Execute “make” or “make all” in terminal in order to produce the banhammer executable.

3. Run the executable and enter the input that needs to be filtered from the stdin stream.

5. In order to scan-build the source file, run “make scan-build” in the terminal.

6. In order to clean up (remove object and executable files), run “make clean” in the terminal.

7. In order to format files, run “make format” in the terminal.

This is a part of a lab given by Prof. Darrell Long.

