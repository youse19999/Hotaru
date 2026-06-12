chcp 65001
git clone https://github.com/conan-io/cmake-conan.git -b develop2
conan install . --build=missing --update  -s build_type=Debug -s compiler.cppstd=17
cmake -B build -S . -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=./cmake-conan/conan_provider.cmake -DCMAKE_BUILD_TYPE=Debug 
cmake . -B build
pause