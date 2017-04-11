# heckel-diff

Project for learning C++. Inspired by IGListKit (https://github.com/Instagram/IGListKit).

Based on Paul Heckel's linear time diffing paper (http://documents.scribd.com/docs/10ro9oowpo1h81pgh1as.pdf)

Feedback gratefully received.

## Build
If you have CLion - checkout the project and open, otherwise:

- Open a terminal
- clone the project to a folder
- `cd` to checkout_folder/heckel_diff
- ensure you've got CMake installed
- `cmake -DCMAKE_BUILD_TYPE=DEBUG -H. -Bbuild && cd build && make && cd ../`

## Modify
The `Algorithm<T>` currently accepts `std::string`, `size_t` and `uint32_t`
- `vi example/main.cpp` (or favourite editor) and do your modifications
- `cmake -H. -Bbuild && cd build && make`

## Run
- tests `./build/test/heckel_diff_tests`
- example `./build/example/heckel_diff_example`

### Notes
Even though the intention is for a library project, the `src/` folder has an binary and a `main.cpp`. If the above commands are run on Linux, a valgrind target will be available that runs against the reference manual string example. 

The tests have a wall_clock and cpu_clock (`TEST(HeckelDiff, Benchmark)`) test set to expect 5000 diffs to run in no greater than wall_clock 16.67ms (60fps). You may have to adjust this as your computer requires.

I use some of the non NSIndexPath test conditions from IGListKit (https://github.com/Instagram/IGListKit).

CPPLint.cfg is available.