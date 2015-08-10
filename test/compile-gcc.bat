g++ -I ..\include -I ..\..\..\GaussianLib\repository\include -std=c++11 ..\sources\MeshGeneratorCuboid.cpp ..\sources\TriangleMesh.cpp -o gausslib.o
ar rcs gausslib.a gausslib.o