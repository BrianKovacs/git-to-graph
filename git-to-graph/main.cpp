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
#include <iterator>
#include <map>


using namespace std;

void parseCommit(string str, map <string, int> &nodes);
void addNode(string str, map <string, int> &nodes);

int main(int argc, const char * argv[]) {
    
    // Validate arguments
    string cmd;
    if (argc == 1)
        cmd = "git log --pretty=format:'%h - %ae' --name-status --reverse";
    else if (argc == 2)
        cmd = "git -C " + string(argv[1]) + " log --pretty=format:'%h - %ae' --name-status --reverse";
    else {
        cout << "Improper arguments.";
        return 0;
    }
    
    // Read the log into a stringstream
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
    
    // Maps to store all nodes and edges
    map <string, int> nodes;
    map <string, int> edges;
    
    // Parse the stringstream and extract each commit
    int count = 0;
    string line;
    string commit = "";
    string hash = "0123456789abcdef";
    while (getline(stream, line)) {
        
        if (hash.find(line[0]) != std::string::npos) {
            // Reached new commit line
            if (!commit.empty()) {
                // Create edges from previous commit
            }
            commit = line;
            count ++;
        } else {
            if (string("AMDR").find(line[0]) != std::string::npos) {
                // Add node for file
                addNode(line, nodes);
            }
            // Add file to current commit
            commit+= '\n' + line;
        }
    }
    cout << "Total: " << count << endl;
    
    return 0;
}

void parseCommit(string str, map <string, int> &nodes) {
    
    cout << str << endl;
}

void addNode(string str, map <string, int> &nodes) {
    cout << str << endl;
}
