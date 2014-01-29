CXX = g++
CXXFLAGS = -Wall -O3 -Iinclude/

OBJ=$(addprefix build/, raytrace.o lodepng.o primitives.o scene.o trace.o)

raytrace: $(OBJ)
	$(CXX) $(CXXFLAGS) -o raytrace $(OBJ)

clean:
	rm -fr raytrace build

build/%.o: src/%.cpp include/*.h | build/
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
build/:
	mkdir build/