rm -rf CMakeFiles CMakeCache.txt cmake_install.cmake Makefile bin lib

cd base
./clean.sh
cd ..

cd log
./clean.sh
cd ..

cd net
./clean.sh
cd ..

echo "iak OK."
