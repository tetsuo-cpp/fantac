# FantaC
FantaC is an LLVM frontend for the C programming language.
## Progress
Unusable.
## Dependencies
* CMake.
* LLVM 5.
* spdlog.
## Build
Bring in Git submodules.
```
git submodule init
git submodule update
```
Use CMake to generate a platform specific build script and then invoke that.
```
cmake .
make
```
## Usage
You can generate LLVM IR for a C source file like so.
```
./fantac [FILE] [LOGGING_LEVEL]
```
See ```build.sh``` for an example of how you can use this in conjunction with ```llc``` to compile to an executable.
## References
* [9cc by Rui Ueyama](https://github.com/rui314/9cc).
* [QCC by uint256_t](https://github.com/maekawatoshiki/qcc).
