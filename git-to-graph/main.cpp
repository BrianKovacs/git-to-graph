//
//  main.cpp
//  git-to-graph
//
//  Created by Brian Kovacs on 11/26/17.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

using namespace std;


void readLog(string cmd) {
    
    stringstream stream;
    FILE * file;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");
    
    file = popen(cmd.c_str(), "r");
    if (file) {
        while (!feof(file))
            if (fgets(buffer, max_buffer, file) != NULL) stream << buffer;
        pclose(file);
    }
    
    int count = 0;
    string line;
    while (getline(stream, line)) {
        if (line[0] == 'A') {
            cout << line << endl;
            count ++;
        }
    }
    cout << "Total: " << count << endl;
    
}

int main(int argc, const char * argv[]) {
    
    // insert code here...
    string cmd;
    if (argc == 1)
        cmd = "git log --pretty=format:'%h - %ae' --name-status --reverse";
    else if (argc == 2)
        cmd = "git -C " + string(argv[1]) + " log --pretty=format:'%h - %ae' --name-status --reverse";
    else {
        cout << "Improper arguments.";
        return 0;
    }
    
    readLog(cmd);
    
    
    return 0;
}
