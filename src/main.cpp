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
    string command2, command3;

    if(argc >= 3)
        command2 = argv[2];
    
    if(argc >= 4)
        command3 = argv[3];

    VCS vcs;
    
    if (command1 == "init") {
        vcs.init();
    } else if (command1 == "status") {
        vcs.status();
    } else if (command1 == "add") {
        string filename = argv[2];
        vcs.add(filename);
    } else if (command1 == "commit") {
        cout << "Bruh" <<endl;
        if(command2 == "-m"){
            cout << "Committing" <<endl;
            string message = command3;
            vcs.commit(message);
        }
    } else {
        cerr << "Error: Invalid command!! " << endl;
        return 1;
    }

}

