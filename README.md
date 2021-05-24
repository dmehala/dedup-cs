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
$ cd build && ctest
$ # CPU: AMD Ryzen 7 3700X@3593 Mhz, Memory: 32GB
$ time ./build/dedup movies.tsv
Movie collection size: 558458
101244 duplicates found, check result.tsv for more details.

real    0m6.069s
user    0m4.169s
sys     0m0.872s

$ cat result.tsv | head -n 10
ec2adfef-1bb7-4575-80db-3af811f91847    66046834-07ae-404d-b9bb-363c36d9578d
48bc77bb-5241-4f4f-8cfc-1a40325d9e55    ee8f8272-7afe-4c2b-8086-df3d323080ed
19e8bdb7-98fa-467f-a3be-0f5d24fc22ea    61eec0f8-255f-4cec-923a-7ff393541070
8793e4c4-3c37-4df8-9e96-a4457802c2e0    fcfc01c0-384f-4dcf-8894-f4ce3322e258
66513e81-a1ed-4b33-bef7-a1d5c9dcd503    790cd1ed-e82a-4697-8790-9cea72c0442e
726c7b77-ea62-4b74-85f6-df797ebd8209    0e8f9b23-0806-4038-a8fd-c536c4322d9b
8f19b8aa-8567-4576-8509-211b31458ec8    8a5c7b51-ee7c-451b-a514-f6e535bdd492
a19c4fa8-cba5-42bc-9bc7-1f2402592ec5    809e1a1e-7f85-4ee5-9ca3-147b401ace2a
d0513672-e759-4f35-a54f-edd0df993377    854265c1-94c8-472d-9945-3486ecd3a829
37f4262c-c907-4e9a-bb6f-cfb0b8bcdc2e    a1ccda1b-32b7-4147-afbc-de892617561c
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

## Blocking algorithm
This is where I failed. I use a standard blocking algorithm, but there is so many blocking techniques (block building + block refinement).