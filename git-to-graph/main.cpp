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
#include <vector>
#include <map>

using namespace std;

typedef map<string, int> NodeMap;
typedef map<string, int> EdgeMap;

bool isHash(const string &str);
void addNode(const string &file, const string &status, int commit, NodeMap &nodes);
void createEdges(const vector<string> &edgeGroup, int commit);
void addEdge(const string &edge);

int main(int argc, const char * argv[])
{
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
    FILE * console;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");
    console = popen(cmd.c_str(), "r");
    if (console) {
        while (!feof(console))
            if (fgets(buffer, max_buffer, console) != NULL) stream << buffer;
        pclose(console);
    }
    
    // Maps to store all nodes and edges
    NodeMap nodes;
    EdgeMap edges;
    
    
    int c = 0;
    int f = 0;
    string line;
    string file;
    string status;
    vector<string> edgeGroup;
    // Parse the stringstream and extract each commit
    while (getline(stream, line)) {
        // Check if first line starts with hash (so this is a new commit)
        if (isHash(line)) {
            // Create edges from the previous commit
            createEdges(edgeGroup, c);
            // Empty the edge group
            edgeGroup.clear();
            // Advance the commit number
            ++c;
        } else if (!line.empty()) {
            // If line isn't empty, add the file to the current commit
            // Separate file and status
            file = line.substr(line.find_first_of('\t')+1);
            status = line.substr(0, line.find_first_of('\t'));
            // Add file to node map
            addNode(file, status, c, nodes);
            // Add file to current enge group
            edgeGroup.push_back(file);
            // Advance the commit number
            ++f;
        }
    }
    
    /*
    cout << "Commits:\t" << c << endl << "Files:\t" << f << endl;
    NodeMap::iterator itr;
    cout << "\nNode Map: \n";
    cout << "\tDEG\tFILE\n";
    for (itr = nodes.begin(); itr != nodes.end(); ++itr)
    {
        cout << '\t' << itr->second
        << '\t' << itr->first << '\n';
    }
    cout << endl;
     */
    
    return 0;
}

bool isHash(const string &s)
{
    return ((s[0] >= '0' && s[0] <= '9') || (s[0] >= 'a' && s[0] <= 'f'));
}

void addNode(const string &file, const string &status, int commit, NodeMap &nodes)
{
    if (status[0] == 'R') {
        // File was renamed
        // ...
    } else {
        NodeMap::iterator lb = nodes.lower_bound(file);
        if(lb != nodes.end() && !(nodes.key_comp()(file, lb->first)))
        {
            // key already exists
            // update lb->second
            lb->second++;
        } else {
            // the key does not exist in the map
            // Use lb as a hint to insert, so it can avoid another lookup
            nodes.insert(lb, NodeMap::value_type(file, 1));
        }
    }
}

void createEdges(const vector<string> &edgeGroup, int commit)
{
    cout << "Commit " << commit << endl;
    vector<string>::const_iterator i;
    for(i=edgeGroup.begin(); i!=edgeGroup.end(); ++i){
        cout << '\t' << (*i) << endl;
    }
}

void addEdge(const string &edge)
{
    
}

