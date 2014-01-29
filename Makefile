CXX = g++
CXXFLAGS = -Wall -O3 -Iinclude/

OBJ=$(addprefix build/, raytrace.o lodepng.o primitives.o scene.o trace.o)

raytrace: $(OBJ)
	$(CXX) $(CXXFLAGS) -o raytrace $(OBJ)

clean:
	rm -fr raytrace build/*.o

build/%.o: src/%.cpp include/*.h
	$(CXX) $(CXXFLAGS) -c $< -o $@