remove -r build
mkdir build
cmake -DCMAKE_BUILD_TYPE=Release -S . -B ./build
cmake --build ./build/
./build/Server 3213 5 202400 ./

#ps aux | grep Server
#pkill Server