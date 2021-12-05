export CPLUS_INCLUDE_PATH=/usr/local/opt/llvm/include/c++/v1:/Library/Developer/CommandLineTools/SDKs/MacOSX10.15.sdk/usr/include

sudo cmake .
make
cd cppFiles
./run