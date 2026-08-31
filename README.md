# Cache Lab
CS:APP Cache Simulator & Cache-Friendly Matrix Transpose Optimizer.

### What the project is
A systems programming project from the classic CMU **CS:APP (Computer Systems: A Programmer's Perspective)** curriculum, implemented in C. The project consists of two core components: building a software-level cache simulator that tracks memory access behavior and designing cache-conscious matrix transposition algorithms to minimize cache miss penalties.

### Aim
The primary aim of this project is to develop an in-depth understanding of the hardware memory hierarchy. It focuses on the mechanics of CPU caches (sets, lines, blocks), cache replacement policies (LRU), bitwise memory address decomposition, and writing software optimized for spatial and temporal locality.

### Key Features
* **Cache Simulator (`csim.c`):** A configurable, trace-driven simulator supporting arbitrary cache parameters ($S = 2^s$ sets, $E$ lines per set, $B = 2^b$ byte block size).
* **Valgrind Trace Parsing:** Accurately processes standard memory trace files containing load (`L`), store (`S`), data modify (`M`), and instruction (`I`) operations.
* **LRU Replacement Policy:** Accurately tracks access recency across cache lines to handle evictions deterministically.
* **Optimized Matrix Transposition (`trans.c`):** Specialized transpose implementations minimizing cache misses for multiple matrix sizes ($32 \times 32$, $64 \times 64$, and $61 \times 67$).
* **Automated Evaluation:** Integrated test scripts and driver evaluation matching the official CS:APP autograding benchmarks.

### Challenges
* **Diagonal Conflict Misses in Transposition:** Directly transposing matrices creates severe conflict misses along diagonals because rows of the source matrix and columns of the destination matrix map to identical cache sets.
    * **Solution:** Applied loop blocking (tiling) combined with temporary local register buffering to hold entire sub-blocks, eliminating conflict misses and maximizing spatial locality.
* **$64 \times 64$ Matrix Conflict Bottlenecks:** A larger square matrix easily overflows standard $L_1$ cache line capacity when using basic square blocks (e.g., $8 \times 8$).
    * **Solution:** Divided the problem into $4 \times 4$ sub-blocks and reorganized memory access sequences so data is loaded and transposed within registers before writing back to memory.
* **Dynamic Bitmask Extraction:** Deconstructing 64-bit memory addresses dynamically based on runtime parameters.
    * **Solution:** Implemented bitwise operations (masking and shifting) to cleanly isolate the tag, set index, and block offset bits.

### Setup

1. **Clone the repository:**
```bash
git clone [https://github.com/Szafter12/Cache-Lab-CSAPP.git](https://github.com/Szafter12/Cache-Lab-CSAPP.git)
cd Cache-Lab-CSAPP
```
2. **Compile the project:**
```bash
make
```
3. **Run the Cache Simulator:**
```bash
./csim -s 4 -E 2 -b 4 -t traces/yi.trace
```
4. **Run the full test suite:**
```bash
./test-csim
./test-trans -M 32 -N 32
./test-trans -M 64 -N 64
./test-trans -M 61 -N 67
```

### Technical concepts used
- Cache Memory Architecture: Tag matching, set indexing, valid bits, and block offsets.
- Locality of Reference: Exploiting spatial and temporal locality to maximize CPU throughput.
- Cache-Conscious Optimization: Loop blocking/tiling, register-level buffering, and memory access reordering.
- Bitwise Address Parsing: Bit shifting and bitmasking in C for 64-bit address manipulation.
- C Systems Programming: Dynamic memory management, file I/O, and getopt command-line argument parsing.
