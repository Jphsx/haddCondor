all: haddCondor.cpp
	g++ haddCondor.cpp -o haddCondor.x
	
hadd: haddjob.cpp
	g++ haddjob.cpp -o haddjob.x

