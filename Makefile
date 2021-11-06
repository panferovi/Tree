CXX = g++
CXXFLAGS = -o $@

tree: main.cpp tree.hpp
	$(CXX) $(CXXFLAGS) $<

gentest: ./Tests/test.cpp tree.hpp
	$(CXX) $(CXXFLAGS) $<

testcopy: testcopy.cpp tree.hpp
	$(CXX) $(CXXFLAGS) $<