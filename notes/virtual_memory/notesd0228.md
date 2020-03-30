# CS3650: Virtual Memory and Cache

Upcoming assignmet: Memory allocator

You'll be implementing malloc and free

Memory allocator slides are linked in today's notes

## Page Tables

- Each process gets its own virtual address space
- Pages are 4k chunks of memroy and are the unit of memory management
- This means that each process needs its own page table to translate virtual addresses to physical addresses.

32-bit page tables (for i368)

-How many potential 4k pages in 32-bit address space?
	- Each page is 2^12, that leaves 20 bits for page number.
	- So we can have 2^20 pages = 1M

Simple page tables:

- We need to map (virtual page #) to a PTE, 
  which is (physical page number, flags)
- Flags are stuff like valid?, present?, r/w/x?
- PTEs on 32-bit x86 are 32 bits
- Easiest way to map an integer to some data is an array
	- The page table for each process is 4 MB
	- this probably isn't how page tables really work

Optimizing page tables:

- Plan A: Hash map
	- Solves the space problem
	- Hash map operations are amortized constant time (sometimes constant, sometimes linear)
	- You can build a realtime hash table
	- Page tables need to be pretty simple, since we're going to implement them in hardware and don't want bugs
- Plan B: Binary Search Tree
	- Operations are all log(N)
	- Actually, we know log(N) = 20 so O(20) = O(1)
	- But 20's kind of a big constant
	- This doesn't help with complexity

How it actual works: Multi-level tables
	- Technically, this is a kind of Trie
	- It's two levels of arrays
	- The 20 bit[of the virtual memory address] page number is split into 2 parts:
		- 10 bits: Index into page directory array
		- 10 bits: Index into 2nd-level page table array
	- Page directory maps index -> address of 2nd level table.
		-Has a valid? bit, if there's no addresses with that prefix there's no 2nd level table

Example: A process with 4 pages: stack, heap, text, data
	- With single level table, this takes 4 MB
	- With multi level table, this takes [4k for the page directory + 4k for at least one 2nd level table, up to 16k for 4 2nd level tables]
		- total is 8 - 20 kB

## 640bit amd64 machines

- Memory addresses are 64 bits
	- Address space size: 16 EB (quintillion)
- Wait, no, address are really 48 bits stored in 64 bit integer
	-  

## Predicting Performation

### Arithmetic

`lscpu` : 
Model:				   Intel(R) Core(TM) i7-9850H CPU @ 2.60GHz
CPU MHz:               3658.988
CPU max MHz:           4600.0000    ---> 4.5 GHz (every second, the computer does 4 billion clock cycles)
CPU min MHz:           800.0000

We can do 1 addition per clock cycle `for (;;) {a += 5}`

Complication: There are 4 ALUs, each of those can do an integer arithmetic operation each cycle
(as long as there are no dependecies) `for (;;) {a += 5; b += 6 c += 7 d +=8}` NOT `for (;;) {a += 5; b += 6a c += 7 d +=8}`

### Memory

L1d cache:             32K
	- Latency: 1ns = 4 cycles
	- Throughput: 1000 GB/s
L1i cache:             32K

L2 cache:              256K
	- Latency: 3ns = 12 cycles
	- Throughput: ...

L3 cache:              12288K
	- Latency: 11ns = 44 cycles
	- Throughput: ...

System Memory:			16G
	- Latency: 60ns = 240 clocks
	- Throughput: 20 GB/2

Cache operates in cache lines (64 bytes)
	- 1B / (16 ints per cache line) = 60M
	- 60M * 60 ns = 3600 M x ns = 3.6 seconds

Esitimates:
	- Just the adds: 1/8th of a second 
	- all the arithmetic: 1/8th of a second
	- Memory throughput: 1/3rd of a second
	- Cache misses: 3.6 seconds
















