# Build and run srcReport

.PHONY: all
all : srcReport

srcReport : srcReport.o XMLParser.o
	g++ -O3 srcReport.o XMLParser.o -o srcReport

srcReport.o : srcReport.cpp XMLParser.hpp
	g++ -O3 -std=c++11 -c srcReport.cpp

XMLParser.o : XMLParser.cpp
	g++ -O3 -std=c++11 -c XMLParser.cpp

# test file, shipped gzipped
libxml2.xml : libxml2.xml.gz
	gunzip --keep libxml2.xml.gz

.PHONY: run
run : srcReport libxml2.xml
	./srcReport < libxml2.xml

.PHONY: clean
clean :
	@rm -f srcReport srcReport.o XMLParser.o

.PHONY: cleanall
cleanall :
	@make clean
	@rm -f libxml2.xml
