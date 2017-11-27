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

struct Target {
    int id;
    int weight;
};

//struct nodeData {
//    int id;
//    vector<target> targets;
//};

typedef map<string, int> NodeMap;
typedef map<int, vector<Target>> EdgeMap;

bool isHash(const string &str);
int addNode(const string &file, const string &status, int commit, NodeMap &nodes);
void createEdges(const vector<int> &edgeGroup, int commit, EdgeMap &edges);
void addEdge(int source, int target, EdgeMap &edges);

int main(int argc, const char * argv[])
{
    cout << "Start\n";
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
            if (status[0] != 'R') {
                // Add file to node map
                edgeGroup.push_back(addNode(file, status, c, nodes));
                // Add file to current enge group
//                edgeGroup.push_back(file);
                // Advance the commit number
                ++f;
            }
        }
    }
    
    cout << "Commits:\t" << c << endl << "Files:\t" << f << endl;
    
    /*
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
    EdgeMap::iterator j;
    cout << "\nEdge Map: \n";
    cout << "\tW\tEDGE\n";
    for (j = edges.begin(); j != edges.end(); ++j)
    {
//        cout << '\t' << j->second
//        << '\t' << j->first << '\n';
        cout << j->first << endl;
        vector<Target>::const_iterator k;
        for(k=j->second.begin(); k!=j->second.end(); ++k){
            cout << '\t' << (*k).id << endl;
        }
    }
    cout << endl;
    */
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
        // update lb->second
//        lb->second++;
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
    
//    cout << "Commit " << commit << endl;
//    vector<int>::const_iterator t;
//    for(t=edgeGroup.begin(); t!=edgeGroup.end(); ++t){
//        cout << '\t' << (*t) << endl;
//    }
    
    /*
    string edge;
    vector<string>::const_iterator i;
    vector<string>::const_iterator j;
    for(i=edgeGroup.begin(); i!=edgeGroup.end(); ++i){
        for(j=i; j!=edgeGroup.end(); ++j){
//        cout << "\t\t" << (*i) << "<--->" << (*j) << endl;
            edge = (*i) + ":" + (*j);
            addEdge(edge, edges);
        }
    }
     */
    vector<int>::const_iterator i;
    vector<int>::const_iterator j;
    for(i=edgeGroup.begin(); i!=edgeGroup.end(); ++i){
        for(j=i; j!=edgeGroup.end(); ++j){
            //        cout << "\t\t" << (*i) << "<--->" << (*j) << endl;
//            edge = (*i) + ":" + (*j);
            addEdge((*i), (*j), edges);
        }
    }
}

void addEdge(int source, int target, EdgeMap &edges)
{
    /*
    EdgeMap::iterator lb = edges.lower_bound(edge);
    if(lb != edges.end() && !(edges.key_comp()(edge, lb->first)))
    {
        // key already exists
        // update lb->second
        lb->second++;
    } else {
        // the key does not exist in the map
        // Use lb as a hint to insert, so it can avoid another lookup
        edges.insert(lb, EdgeMap::value_type(edge, 1));
    }
     */
    
    EdgeMap::iterator lb = edges.lower_bound(source);
    if(lb != edges.end() && !(edges.key_comp()(source, lb->first)))
    {
        // key already exists
        // update lb->second
//        lb->second++;
        
        Target t = {target, 1};
        lb->second.push_back(t);
    } else {
        // the key does not exist in the map
        // Use lb as a hint to insert, so it can avoid another lookup
        edges.insert(lb, EdgeMap::value_type(source, vector<Target>()));
        Target t = {target, 1};
        lb->second.push_back(t);
    }
}

