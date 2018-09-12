# heckel-diff

Project for learning C++. Inspired by IGListKit (https://github.com/Instagram/IGListKit).

Based on Paul Heckel's linear time diffing paper (http://documents.scribd.com/docs/10ro9oowpo1h81pgh1as.pdf)

Feedback gratefully received.

## Modify
The `Algorithm<T>` currently accepts `std::string`, `size_t` and `uint32_t`
- `vi example/main.cpp` (or favourite editor) and do your modifications
- `cmake -H. -Bbuild && cd build && make`

### Notes
The tests have a wall_clock and cpu_clock (`TEST(HeckelDiff, Benchmark)`) test set to expect 1600 diffs to run in no greater than wall_clock 16.67ms (60fps). You may have to adjust this as your computer requires.

I use some of the non NSIndexPath test conditions from IGListKit (https://github.com/Instagram/IGListKit).

CPPLint.cfg is available.
