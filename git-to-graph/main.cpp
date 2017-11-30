//
//  main.cpp
//  git-to-graph
//
//  Created by Brian Kovacs on 11/26/17.
//

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iterator>
#include <vector>
#include <map>

using namespace std;

typedef map<string, int> NodeMap;
typedef map<int, map<int, int>> EdgeMap;

bool isHash(const string &str);
int addNode(const string &file, const string &status, int commit, NodeMap &nodes);
void createEdges(const vector<int> &edgeGroup, int commit, EdgeMap &edges);
void addEdge(int source, int target, EdgeMap &edges);
void writeToFile(string outFile, const NodeMap &nodes, const EdgeMap &edges);

int main(int argc, const char * argv[])
{
    // Validate arguments
    string cmd;
    string outFile;
    if (argc == 2) {
        cmd = "git log --pretty=format:'%h - %ae' --name-status --reverse";
        outFile = string(argv[1]);
    } else if (argc == 3) {
        cmd = "git -C " + string(argv[1]) + " log --pretty=format:'%h - %ae' --name-status --reverse";
        outFile = string(argv[2]);
    } else {
        cout << "usage: git-to-graph out_file.gexf" << endl;
        cout << "usage: git-to-graph git_directory out_file.gexf" << endl;
        return 0;
    }
    
    cout << "Start\n";

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
    vector<int> edgeGroup;
    // Parse the stringstream and extract each commit
    while (getline(stream, line)) {
        // Check if first line starts with hash (so this is a new commit)
        if (isHash(line)) {
            // Create edges from the previous commit
            sort(edgeGroup.begin(), edgeGroup.end());
            createEdges(edgeGroup, c, edges);
            // Empty the edge group
            edgeGroup.clear();
            // Advance the commit number
            ++c;
        } else if (!line.empty()) {
            // If line isn't empty, add the file to the current commit
            // Separate file and status
            file = line.substr(line.find_first_of('\t')+1);
            status = line.substr(0, line.find_first_of('\t'));
            // If status is not rename...
            if (status[0] != 'R' && status[0] !='D') {
                // Add file to node map and to current edge group
                edgeGroup.push_back(addNode(file, status, c, nodes));
                // Advance the commit number
                ++f;
            }
        }
    }

    cout << "Commits:\t" << c << endl << "Files:\t" << f << endl;

    /*
    // Print all nodes
    NodeMap::iterator i;
    cout << "\nNode Map: \n";
    cout << "\tDEG\tFILE\n";
    for (i = nodes.begin(); i != nodes.end(); ++i)
    {
        cout << '\t' << i->second
        << '\t' << i->first << '\n';
    }
    cout << endl;
    */

    /*
    // Print all edges
    EdgeMap::iterator j;
    cout << "\nEdge Map: \n";
    cout << "\tW\tEDGE\n";
    for (j = edges.begin(); j != edges.end(); ++j)
    {
        cout << j->first << endl;
        map<int, int>::const_iterator k;
        for(k=j->second.begin(); k!=j->second.end(); ++k){
            cout << "  -" << k->first << ':' << k->second << endl;
        }
    }
    cout << endl;
    */

    writeToFile(outFile, nodes, edges);

    cout << "End\n";
    return 0;
}

bool isHash(const string &s)
{
    return ((s[0] >= '0' && s[0] <= '9') || (s[0] >= 'a' && s[0] <= 'f'));
}

int addNode(const string &file, const string &status, int commit, NodeMap &nodes)
{
    static int i = 0;
    NodeMap::iterator lb = nodes.lower_bound(file);
    if(lb != nodes.end() && !(nodes.key_comp()(file, lb->first)))
    {
        // key already exists
        return lb->second;
    } else {
        // the key does not exist in the map
        // Use lb as a hint to insert, so it can avoid another lookup
        nodes.insert(lb, NodeMap::value_type(file, ++i));
        return i;
    }
}

void createEdges(const vector<int> &edgeGroup, int commit, EdgeMap &edges)
{
    /*
    // Print all fils grouped by commit
    cout << "Commit " << commit << endl;
    vector<int>::const_iterator t;
    for(t=edgeGroup.begin(); t!=edgeGroup.end(); ++t){
        cout << '\t' << (*t) << endl;
    }
     */
    vector<int>::const_iterator i;
    vector<int>::const_iterator j;
    for(i=edgeGroup.begin(); i!=edgeGroup.end(); ++i){
        for(j=i; j!=edgeGroup.end(); ++j){
            addEdge((*i), (*j), edges);
        }
    }
}

void addEdge(int source, int target, EdgeMap &edges)
{
    EdgeMap::iterator lb = edges.lower_bound(source);
    if(lb != edges.end() && !(edges.key_comp()(source, lb->first)))
    {
        // key already exists
        // update lb->second
        map<int, int>::iterator lb2 = lb->second.lower_bound(target);
        if(lb2 != lb->second.end() && !(lb->second.key_comp()(target, lb2->first)))
        {
            lb2->second++;
        } else {
            lb->second.insert(pair<int, int>(target, 1));
        }
    } else {
        // the key does not exist in the map
        // Use lb as a hint to insert, so it can avoid another lookup
        map<int, int> m;
        m.insert(pair<int, int>(target, 1));
        edges.insert(lb, EdgeMap::value_type(source, m));
    }
}

void writeToFile(string outFile, const NodeMap &nodes, const EdgeMap &edges)
{
    // Add file extension if missing
    if (outFile.size() <=5) {
        outFile.append(".gexf");
    } else if (outFile.substr(outFile.size()-5) != ".gexf") {
        outFile.append(".gexf");
    }
    
    // Open file for writing
    ofstream file (outFile);
    if (file.is_open())
    {
        file << R"(<?xml version="1.0" encoding="UTF-8"?>
<gexf xmlns="http://www.gexf.net/1.3" version="1.3" xmlns:viz="http://www.gexf.net/1.3/viz" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.gexf.net/1.3 http://www.gexf.net/1.3/gexf.xsd">
  <graph mode="static" defaultedgetype="undirected">)" << endl;

        // Print all nodes
        file << "    <nodes>" << endl;
        NodeMap::const_iterator i;
        for (i = nodes.begin(); i != nodes.end(); ++i)
        {
            file << "      <node id='" << i->second <<
            "' lable='" << i->first << "'/>" << endl;
        }
        file << "    </nodes>" << endl;

        // Print all edges
        file << "    <edges>" << endl;
        EdgeMap::const_iterator j;
        int id = 0;
        for (j = edges.begin(); j != edges.end(); ++j)
        {
            map<int, int>::const_iterator k;
            for(k=j->second.begin(); k!=j->second.end(); ++k){
                file << "      <edge id='" << ++id
                << "' source='" << j->first
                << "' target='" << k->first
                << "' weight='" << k->second << "'/>" << endl;
            }
        }
        file << "    </edges>" << endl;
        file << "  </graph>" << endl;
        file << "</gexf>" << endl;

        file.close();
    }
    else cout << "Unable to open file";
}
