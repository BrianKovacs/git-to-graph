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

typedef map<string, int> NodeMap;
typedef map<string, int> EdgeMap;

bool isHash(const string &str);
void addNode(const string &str, int commit, NodeMap &nodes);

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
    NodeMap nodes;
    map <string, int> edges;
    
    // Parse the stringstream and extract each commit
    int c = 0;
    int f = 0;
    string line;
    while (getline(stream, line)) {
        // Check if first line starts with hash (so this is a new commit)
        if (isHash(line)) {
            // Create edges from the previous commit
            //...
            
            // Advance the commit number
            ++c;
        } else if (!line.empty()) {
            // If line isn't empty, add the file to the current commit
            addNode(line, c, nodes);
            ++f;
        }
    }
    cout << "Commits:\t" << c << endl << "Files:\t" << f << endl;
    
    // printing map gquiz1
    NodeMap::iterator itr;
    cout << "\nNode Map: \n";
    cout << "\tDEG\tFILE\n";
    for (itr = nodes.begin(); itr != nodes.end(); ++itr)
    {
        cout << '\t' << itr->second
        << '\t' << itr->first << '\n';
    }
    cout << endl;
    
    return 0;
}

bool isHash(const string &s) {
    return ((s[0] >= '0' && s[0] <= '9') || (s[0] >= 'a' && s[0] <= 'f'));
}

void addNode(const string &str, int commit, NodeMap &nodes) {
    if (str[0] == 'R') {
        // File was renamed
        // ...
    } else {
        //cout << str << endl;
        //cout << str.substr(str.find_first_of('\t')+1) << endl;
        
        // Remove status and \t character from the file
        string file = str.substr(str.find_first_of('\t')+1);
        NodeMap::iterator lb = nodes.lower_bound(file);
        
        if(lb != nodes.end() && !(nodes.key_comp()(file, lb->first)))
        {
            // key already exists
            // update lb->second if you care to
        }
        else
        {
            // the key does not exist in the map
            // Use lb as a hint to insert, so it can avoid another lookup
            nodes.insert(lb, NodeMap::value_type(file, 1));
        }
    }
}

