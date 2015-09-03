g++ -I ..\include -I ..\..\..\GaussianLib\repository\include -std=c++11 -c ..\sources\MeshGeneratorCuboid.cpp ..\sources\TriangleMesh.cpp
ar rcs libgeomlib.a TriangleMesh.o MeshGeneratorCuboid.o
g++ -I ..\include -I ..\..\..\GaussianLib\repository\include -std=c++11 -c test1.cpp -o test1.o
g++ -std=c++11 test1.o -L .\ -lgeomlib -o test1-gcc.exe
g++ -I ..\include -I ..\..\..\GaussianLib\repository\include -std=c++11 -c test2.cpp -o test2.o
g++ -std=c++11 test2.o -L .\ -lgeomlib -lopengl32 -o test2-gcc.exe