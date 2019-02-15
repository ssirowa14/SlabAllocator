The goal of this project was to design & implement a slab allocator in a C++ procedural library
for memory management on Linux. One part of the project was to make Linux Slab Allocator
and the other part of the project was to make slab allocator thread safe

Readme for SlabAllocator:
1. Open Terminal
2. Locate to directory
3. To run, type g++ libmymem.cpp -lpthread

libmymem.hpp contains all definitions, libmymem.cpp contains the mymalloc() and myfree() functions.