rm -rf CMakeFiles CMakeCache.txt cmake_install.cmake Makefile bin lib

cd base
./clean.sh
cd ..

cd net
./clean.sh
cd ..

cd frame
./clean.sh
cd ..

echo "iak OK."
