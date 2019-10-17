#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <string>
#include <algorithm> 

/**
 * Creates an Hashmap of mean-core-consumptions starting from a CSV file like (convert_dict.csv)
 * 
 * id,user,job,tot_cores,node_id,req_cores,real_pow
 * 0,user1,user1-job1,16,9,16,213.90723480321014
 * 1,user2,user2-job1,96,9,16,1232.7223322824611
 * 
 * 
 */

namespace mcc_utils{

    class CSVRow
    {
      public:
        std::string const &operator[](std::size_t index) const
        {
            return m_data[index];
        }
        std::size_t size() const
        {
            return m_data.size();
        }

        void readNextRow(std::istream &str)
        {
            std::string line;
            std::getline(str, line);

            std::stringstream lineStream(line);
            std::string cell;

            m_data.clear();
            while (std::getline(lineStream, cell, ','))
            {
                m_data.push_back(cell);
            }
            // This checks for a trailing comma with no data after it.
            if (!lineStream && cell.empty())
            {
                // If there was a trailing comma then add an empty element.
                m_data.push_back("");
            }
        }

      private:
        std::vector<std::string> m_data;
    };

    std::istream &operator>>(std::istream &str, CSVRow &data)
    {
        data.readNextRow(str);
        return str;
    }

    struct Node
    {
        std::pair<double, int> mcc; // <mcc, counts>

        void update(double newMcc){
            mcc.first = getAvg(mcc.first, mcc.second, newMcc);
            mcc.second = mcc.second+1;
        }

        std::string print()
        {
            return "{" + std::to_string(mcc.first) +  "," + std::to_string(mcc.second) + "}";
        }
    };

    struct Job
    {
        std::map<std::string, Node> nodes; // perUserJobNode method
        std::pair<double, int> mcc;        // <mcc, counts>

        void update(double newMcc){
            mcc.first = getAvg(mcc.first, mcc.second, newMcc);
            mcc.second = mcc.second+1;
        }

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
        std::pair<double, int> mcc; // <mcc, counts>
        std::map<std::string, Node> nodes;
        std::map<std::string, Job> jobs;

        void update(double newMcc){
            mcc.first = getAvg(mcc.first, mcc.second, newMcc);
            mcc.second = mcc.second+1;
        }

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
            return perUser + "\n" + perUserNode + "\n" + perUserJob + "\n";
        }
    };

    struct MainMCCmap
    {
        std::map<std::string, Node> nodes; // perNode method
        std::map<std::string, User> users;

        std::pair<double, int> perNode(std::string node){
            try {
                Node *n = &nodes.at(node);
                return n->mcc; // perNode if exist
            } catch (std::out_of_range e) {
                throw e;
            };
        }

        std::pair<double, int> perUser(std::string user){
            try {
                User *u = &users.at(user);
                return u->mcc;
            } catch (std::out_of_range e) {
                throw e;
            };
        }

        std::pair<double, int> perUserNode(std::string user, std::string node){
            try {
                User *u = &users.at(user);
                try {
                    Node *n = &u->nodes.at(node);
                    return n->mcc;
                } catch (std::out_of_range e) {
                    throw e;
                };
            } catch (std::out_of_range e) {
                throw e;
            };
        }

        std::pair<double, int> perUserJob(std::string user, std::string job){
            try {
                User *u = &users.at(user);
                try {
                    Job *j = &u->jobs.at(job);
                    return j->mcc;
                } catch (std::out_of_range e) {
                    throw e;
                };
            } catch (std::out_of_range e) {
                throw e;
            };
        }

        std::pair<double, int> perUserJobNode(std::string user, std::string job, std::string node){
            try {
                User *u = &users.at(user);
                try {
                    Job *j = &u->jobs.at(job);
                    try {
                        Node *n = &j->nodes.at(node);
                        return n->mcc;
                    } catch (std::out_of_range e) {
                        throw e;
                    };
                } catch (std::out_of_range e) {
                    throw e;
                };
            } catch (std::out_of_range e) {
                throw e;
            };
        }

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

    int create_map(std::string inputFile, std::string outputFile)
    {
        MainMCCmap mcc_map;
        mcc_map.users = std::map<std::string, User>();
        mcc_map.nodes = std::map<std::string, Node>();
        //std::ifstream file("convert_dict_copy.csv");
        std::ifstream file(inputFile);
        CSVRow row;
        // int i = 0;
        while (file >> row)
        {
            // std::cout << i;
            // i++;
            // _, user, job, ncores_tot, node_id, ncores, real_pow
            std::string user = row[1];
            std::string job = row[2];
            int ncores_tot = std::stoi(row[3]);
            std::string node = row[4];
            int ncores = std::stoi(row[5]);
            double real_pow = std::stod(row[6]);

            double mcc = real_pow / ncores_tot;

            try {
                Node *n = &mcc_map.nodes.at(node);
                // perNode if exist
                updateNode(n, mcc);
                // std::cout << "node exist" << std::endl;
                // std::cout << n->print() << std::endl;
            } catch (std::out_of_range) {
                // perNode if NOT exist
                mcc_map.nodes[node] = Node();
                mcc_map.nodes[node].mcc = std::pair<double, int>(mcc, 1);
                // std::cout << "node NOT exist" << std::endl;
                // std::cout << mcc_map.nodes[node].print() << std::endl;
            }

            try {
                User *u = &mcc_map.users.at(user);
                // perUser if exist
                updateUser(u, mcc);

                // perUserNode
                try {
                    Node *n = &u->nodes.at(node);
                    // perUserNode if exist
                    updateNode(n, mcc);

                } catch (std::out_of_range) {
                    // perUserNode if NOT exist
                    u->nodes[node] = Node();
                    u->nodes[node].mcc = std::pair<double, int>(mcc, 1);
                }

                // perUserJob
                try {
                    Job *j = &u->jobs.at(job);
                    // per UserJob if exist 
                    updateJob(j, mcc);

                    // perUserJobNode
                    try {
                        Node *n = &j->nodes.at(node);
                        // perUserJobNode if node exist
                        updateNode(n, mcc);

                    } catch (std::out_of_range){
                        // perUserJobNode if node NOT exist
                        j->nodes[node] = Node();
                        j->nodes[node].mcc = std::pair<double, int>(mcc, 1);
                    }

                } catch (std::out_of_range){ // job doesn't exist
                    // perUserJob if NOT exist
                    u->jobs[job]  = Job();
                    u->jobs[job].mcc = std::pair<double, int>(mcc, 1); 
                    // perUserJobNode
                    u->jobs[job].nodes[node] = Node();
                    u->jobs[job].nodes[node].mcc = std::pair<double, int>(mcc, 1); 
                }

            } catch (std::out_of_range){ // user doesn't exist
                // perUser
                mcc_map.users[user] = User();
                mcc_map.users[user].mcc = std::pair<double, int>(mcc, 1);
                // perUserNode
                mcc_map.users[user].nodes[node] = Node();
                mcc_map.users[user].nodes[node].mcc = std::pair<double, int>(mcc, 1);
                // perUserJob
                mcc_map.users[user].jobs[job] = Job();
                mcc_map.users[user].jobs[job].mcc = std::pair<double, int>(mcc, 1);
                // preUserJobNode
                mcc_map.users[user].jobs[job].nodes[node] = Node();
                mcc_map.users[user].jobs[job].nodes[node].mcc = std::pair<double, int>(mcc, 1);

            }

        }

        //std::cout << mcc_map.print() << std::endl;

        std::ofstream myfile;
        myfile.open(outputFile);
        myfile << mcc_map.print();
        myfile.close();
        return 0;
    }


    MainMCCmap loadMap(std::string path)
    {
        MainMCCmap mcc_map;
        mcc_map.users = std::map<std::string, User>();
        mcc_map.nodes = std::map<std::string, Node>();
        std::vector<std::string> values(3);
        std::string currentUser = "";
        std::string currentJob = "";

        int stateA = 0; // 0: we are reading the perNodes info (only the first lines); 1: the perUser
        int stateB = 0; // 0: we are reading the perUser (single value); 1: the perUserNodes; 2: the perUserJobs 
        int stateC = 0; // 0: we are reading the perUserJob

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
                        }
                    break;
                }
            }
            myfile.close();
        } else {
            std::cout << "Unable to open file"; 
        }

        // std::cout << mcc_map.print() << std::endl;

        return mcc_map;
    }
}