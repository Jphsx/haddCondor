

#include <stdio.h>
#include <stdlib.h>
#include <string>

int main(int argc, char *argv[]){
	
	//arg2 = outputname
	//arg3-n = inputnames

	std::string cmd = "hadd -f "+std::string(argv[1])+" ";
	for(int i=2; i<argc; i++){
		cmd = cmd + std::string(argv[i])+" ";
	}
       system(cmd.c_str());
//
	
	
}
