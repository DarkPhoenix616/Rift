#include "../include/vcs.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[]){
    if(string(argv[0]) != "Rift" ){
        cerr << "Error: Invalid VCS!! " << std::endl;
        return 1;
    }

    string command = argv[1];

    VCS vcs;

    if(command == "init"){
        vcs.init();
    }
}

