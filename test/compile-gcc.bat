g++ -I ..\include -I ..\..\..\GaussianLib\repository\include -std=c++11 -c ..\sources\MeshGeneratorCuboid.cpp ..\sources\TriangleMesh.cpp
ar rvs gausslib.a TriangleMesh.o MeshGeneratorCuboid.o
g++ -I ..\include -I ..\..\..\GaussianLib\repository\include -std=c++11 gausslib.a test1.cpp -o test1-gcc.exe