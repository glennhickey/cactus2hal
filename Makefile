rootPath = ./
include ./include.mk

libSources = src/*.cpp
EXCLUDE=$(subst src/importCactusIntoHal.cpp,,${libSources})
libHeaders = inc/*.h 
libTests = tests/*.cpp
libTestsHeaders = tests/*.h 

all : ${libPath}/cactus2halLib.a ${binPath}/importCactusIntoHal ${binPath}/cactus2hal.py ${binPath}/importCactusTests 

clean : 
	rm -f ${libPath}/cactus2halLib.a ${binPath}/importCactusIntoHal ${binPath}/cactus2hal.py ${binPath}/importCactusTests *.o

test: all
	python tests/allTests.py

${libPath}/cactus2halLib.a : ${libSources} ${libHeaders} ${basicLibsDependencies} 
	${cpp} ${cppflags} -I inc -I src -c ${libSources}
	ar rc cactus2halLib.a *.o
	ranlib cactus2halLib.a 
	rm *.o
	mv cactus2halLib.a ${libPath}/

${binPath}/importCactusIntoHal : ${libPath}/cactus2halLib.a src/importCactusIntoHal.cpp
	${cpp} ${cppflags} -I inc -I src -o ${binPath}/importCactusIntoHal ${libPath}/cactus2halLib.a src/importCactusIntoHal.cpp ${basicLibs} ${basicLibs}

${binPath}/importCactusTests :  ${libPath}/cactus2halLib.a ${libTests} ${libTestsHeaders} 
	${cpp} ${cppflags} -I inc -I src -I tests -o ${binPath}/importCactusTests ${libPath}/cactus2halLib.a ${libTests} ${basicLibs}

${binPath}/cactus2hal.py : src/cactus2hal.py
	cp src/cactus2hal.py ${binPath}/cactus2hal.py
	chmod +x ${binPath}/cactus2hal.py
