#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <iterator>
#include <fstream>
#include <vector>
#include <algorithm> 
#include <sstream>

struct Node
{
    // MeanCoreCons node_mcc;
    std::pair<double, int> mcc; // <mcc, counts>

    std::string print()
    {
        return "{" + std::to_string(mcc.first) +  "," + std::to_string(mcc.second) + "}";
    }
};

struct Job
{
    std::map<std::string, Node> nodes; // perUserJobNode method
    // MeanCoreCons user_job_mcc;         // perUserJob method
    std::pair<double, int> mcc;        // <mcc, counts>

    std::string print()
    {
        std::string perUserJob = "{" + std::to_string(mcc.first) +  "," + std::to_string(mcc.second) + "}";

        std::string perUserJobNode = "";
        std::map<std::string, Node>::iterator it = nodes.begin();
        while (it != nodes.end())
        {
            // Accessing KEY from element pointed by it.
            std::string node_id = it->first;
            perUserJobNode += "\t\t" + node_id + ": ";
            // Accessing VALUE from element pointed by it.
            perUserJobNode += it->second.print() + "\n";
            it++;
        }
        return perUserJob + "\n" + perUserJobNode;
    }

};

struct User
{
    // MeanCoreCons user_mcc;      // perUser method
    std::pair<double, int> mcc; // <mcc, counts>
    std::map<std::string, Node> nodes;
    std::map<std::string, Job> jobs;

    std::string print()
    {
        std::string perUser = "{" + std::to_string(mcc.first) +  "," + std::to_string(mcc.second) + "}";

        std::string perUserNode = "";
        std::map<std::string, Node>::iterator it = nodes.begin();
        while (it != nodes.end())
        {
            // Accessing KEY from element pointed by it.
            std::string node_id = it->first;
            perUserNode += "\t" + node_id + ": ";
            // Accessing VALUE from element pointed by it.
            perUserNode += it->second.print() + "\n";
            it++;
        }

        std::string perUserJob = "";
        std::map<std::string, Job>::iterator it2 = jobs.begin();
        while (it2 != jobs.end())
        {
            // Accessing KEY from element pointed by it.
            std::string job_name = it2->first;
            perUserJob += "\t" + job_name + ": ";
            // Accessing VALUE from element pointed by it.
            perUserJob += it2->second.print() + "\n";
            it2++;
        }
        return perUser + "\n" + perUserNode + "\n" + perUserJob;
    }
};

struct MainMCCmap
{
    std::map<std::string, Node> nodes; // perNode method
    std::map<std::string, User> users;

    std::string print()
    {
        std::string users_str = "\n";
        std::map<std::string, User>::iterator it = users.begin();
        while (it != users.end())
        {
            // Accessing KEY from element pointed by it.
            std::string user_name = it->first;
            users_str += "" + user_name + ": ";
            // Accessing VALUE from element pointed by it.
            users_str += it->second.print() + "";
            it++;
        }

        std::string nodes_str = "";
        std::map<std::string, Node>::iterator it2 = nodes.begin();
        while (it2 != nodes.end())
        {
            // Accessing KEY from element pointed by it.
            std::string node_id = it2->first;
            nodes_str += node_id + ": ";
            // Accessing VALUE from element pointed by it.
            nodes_str += it2->second.print() + "\n";
            it2++;
        }
        return nodes_str + "" + users_str + "";
    }
};

double getAvg(double original_mcc, int count, double mcc)
{
    return (original_mcc*count + mcc*1) / (count + 1);
}

void updateNode(Node *n, double mcc)
{
    n->mcc.first = getAvg(n->mcc.first, n->mcc.second, mcc);
    n->mcc.second = n->mcc.second+1;
}

void updateUser(User *u, double mcc)
{
    u->mcc.first = getAvg(u->mcc.first, u->mcc.second, mcc);
    u->mcc.second = u->mcc.second+1;
}

void updateJob(Job *j, double mcc)
{
    j->mcc.first = getAvg(j->mcc.first, j->mcc.second, mcc);
    j->mcc.second = j->mcc.second+1;
}

std::vector<std::string> split(std::string strToSplit, char delimeter)
{
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter))
    {
       splittedStrings.push_back(item);
    }
    return splittedStrings;
}

std::vector<std::string> getValues(std::string str){

    std::vector<std::string> values(3);
    // remove tabs
    str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());

    std::vector<std::string> firstSplit = split(str, ' ');
    firstSplit[0].erase(std::remove(firstSplit[0].begin(), firstSplit[0].end(), ':'), firstSplit[0].end());
    //std::cout << firstSplit[0] + "  " + firstSplit[1] << std::endl;
    firstSplit[1].erase(std::remove(firstSplit[1].begin(), firstSplit[1].end(), '{'), firstSplit[1].end());
    firstSplit[1].erase(std::remove(firstSplit[1].begin(), firstSplit[1].end(), '}'), firstSplit[1].end());

    std::vector<std::string> secondSplit = split(firstSplit[1], ',');

    values[0] = firstSplit[0];
    values[1] = secondSplit[0];
    values[2] = secondSplit[1];

    return values;
}

MainMCCmap loadMap(std::string path)
{
    MainMCCmap mcc_map;
    mcc_map.users = std::map<std::string, User>();
    mcc_map.nodes = std::map<std::string, Node>();
    std::vector<std::string> values(3);
    std::string currentUser = "";
    std::string currentJob = "";

    int stateA = 0;
    int stateB = 0;
    int stateC = 0;

    std::string line;
    std::ifstream myfile (path);
    if (myfile.is_open())
    {
        while ( getline (myfile, line) )
        {
            // std::cout << line << '\n';
            // line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
            switch(stateA)
            {
                case 0: // perNode
                    if(line.empty())
                    {
                        stateA +=1;
                    } else {
                        values = getValues(line);
                        mcc_map.nodes[values[0]].mcc = std::pair<double, int>(std::stod(values[1]), std::stoi(values[2]));
                    }
                break;
                case 1: // perUser
                    switch (stateB)
                    {
                        case 0: // perUser
                            values = getValues(line);
                            currentUser = values[0];
                            mcc_map.users[currentUser].mcc = std::pair<double, int>(std::stod(values[1]), std::stoi(values[2]));
                            stateB += 1;
                        break;
                        case 1: // perUserNode
                            if(line.empty())
                            {
                                stateB += 1;
                            } else {
                                values = getValues(line);
                                mcc_map.users[currentUser].nodes[values[0]].mcc = std::pair<double, int>(std::stod(values[1]), std::stoi(values[2]));
                            }
                        break;
                        case 2: // perUserJob
                            switch(stateC){
                                case 0:
                                    values = getValues(line);
                                    currentJob = values[0];
                                    mcc_map.users[currentUser].jobs[currentJob].mcc = std::pair<double, int>(std::stod(values[1]), std::stoi(values[2]));
                                    stateC += 1;
                                break;
                                case 1: // node serie
                                    if(line.empty())
                                    {
                                        stateC += 1;
                                    } else {
                                        values = getValues(line);
                                        mcc_map.users[currentUser].jobs[currentJob].nodes[values[0]].mcc = std::pair<double, int>(std::stod(values[1]), std::stoi(values[2]));
                                    }
                                break;
                                case 2:
                                    if(line.empty())
                                    {
                                        std::cout << "entratoooooooooo" << std::endl;
                                        stateB = 0;
                                        stateC = 0;
                                        
                                    } else {
                                        values = getValues(line);
                                        currentJob = values[0];
                                        mcc_map.users[currentUser].jobs[currentJob].mcc = std::pair<double, int>(std::stod(values[1]), std::stoi(values[2]));
                                        stateC = 1;
                                    }
                                break;
                            }
                        break;
                        // case 3: // perUserJobNode
                        //     switch (stateC)
                        //     {
                        //         case 0:
                        //             if(line.empty())
                        //             {
                        //                 stateC += 1;
                        //             } else {
                        //                 values = getValues(line);
                        //                 mcc_map.users[currentUser].jobs[currentJob].nodes[values[0]].mcc = std::pair<double, int>(std::stod(values[1]), std::stoi(values[2]));
                        //             }
                        //         break;
                        //         case 1:
                        //             if(line.empty())
                        //             {
                        //                 stateB = 0;
                        //                 stateC = 0;
                        //             } else {
                        //                 values = getValues(line);
                        //                 currentJob = values[0];
                        //                 mcc_map.users[currentUser].jobs[currentJob].mcc = std::pair<double, int>(std::stod(values[1]), std::stoi(values[2]));
                        //                 stateC = 0;
                        //             }
                        //         break;
                        //     }

                        // break;
                    }
                break;
            }
        }
        myfile.close();
    } else {
        std::cout << "Unable to open file"; 
    }

    std::cout << mcc_map.print() << std::endl;

    return mcc_map;
}

int main()
{
    MainMCCmap mcc_map;
    mcc_map = loadMap("mcc_map");

}