# Problem statement
Given a list of movies where each movie is described by two different providers, the task is to identify the duplications.
You can read the full problem statement [here.](Problem.md)

# Prerequisites
* C++17 compiler
* CMake

If you want to read the Jupyter notebook, you will need:
* Jupyter-lab
* Python 3.6+
* All packages in requirements.txt

# How to build
````cpp
$ cmake -S . -B build
$ cmake --build build
````

````cpp
$ cd build && ctest && cd -
$ # CPU: AMD Ryzen 7 3700X@3593 Mhz, Memory: 32GB
$ time ./build/dedup movies.tsv
Movie collection size: 558458
173247 duplicates found, check result.tsv for more details.

real    0m8.276s
user    0m6.312s
sys     0m0.969s

$ cat result.tsv | head -n 10
48926a09-3404-4c3c-94c6-2538717a2b57    3ce62d4f-cac0-48b6-b4b0-69b457f0a683
05b2c1dc-60a7-4c1f-b68d-04a9184af4cd    3ccb48da-0b13-49aa-8ded-98d9152261b7
0aedb3b5-c0d8-46b5-9ee9-ee702decd9f2    8f93875a-50f2-41c8-a1de-f9fa9df5d3d8
17cf6934-9522-401a-9919-6f6b0375bee9    d297c059-21b5-4050-a70b-9c4c3ee503de
fdb26426-58ed-426f-9c24-372bb75033c1    efca3ef7-caec-41ab-93aa-6c443f871e23
215fe554-d8b8-4eaa-a8c7-1f0073de8b31    1d52d85a-9942-4c2d-be8f-e1bbd2f54bde
2bd12e53-78ed-484a-b525-93f4c9d44c62    3154c479-a29c-4549-b65b-56408294b85b
ea665869-d88e-4138-a2ea-86f20e843f0a    83f7f44a-fb12-45ab-a630-01769c64903f
ef0ea64a-4a8c-406a-ae79-905e6c5e1186    cca59175-1465-457e-8a3b-850a550be87c
21fc2ba3-6eec-47a0-adb9-ce2a3cf93cdb    efa6f695-f203-4821-8a91-68c9e846c631
````

# Decision making
_Disclamer_: This is the first time I encounter a deduplication problem, I am no data scientist, neither a data engineer (even if I have a master degre in Business Intelligence).

I suggest to read [data-exploration.ipynb](data-exploration.ipynb) to better understand how I tried to solve the problem.

The code is written in C++17, but I am not used to it, that is probably why I do not use std::string_view more or other new features.

## CSV Reader
The implementation is kind of ok in term of performance but if we wanted to do better and be more scalable, this is how I would do it:
1. Use C API instead of C++ API for file manipulation (fopen, fgets and so on)
2. Memory-map the file and read it by chunks, can be time consuming to make it crossplatfomr
3. Split the works between multiple threads
