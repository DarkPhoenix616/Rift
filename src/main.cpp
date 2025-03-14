#include "../include/vcs.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[]){
    /*if(string(argv[0]) != "Rift" ){
        cerr << "Error: Invalid VCS!! " << std::endl;
        return 1;
    }   */

    string command1 = argv[1];

    if(argc == 3){
        string command2 = argv[2];
    }

    VCS vcs;

    if(command1 == "init"){
        vcs.init();
    }
    else if(command1 == "add"){
        string filename = argv[2];
        vcs.add(filename);
    }
    else if(command1 == "status"){
        vcs.status();
    }
    else{
        cerr << "Error: Invalid command!! " << endl;
        return 1;
    }
}

