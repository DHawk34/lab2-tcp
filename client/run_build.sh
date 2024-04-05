remove -r build
mkdir build
cmake -DCMAKE_BUILD_TYPE=Release -S . -B ./build
cmake --build ./build/
./build/Client

#ps aux | grep Server
#pkill Server