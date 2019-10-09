#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

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

struct MeanCoreCons
{
    std::pair<double, int> value; // <mcc, counts>
};

struct Node
{
    // MeanCoreCons node_mcc;
    std::pair<double, int> mcc; // <mcc, counts>
};

struct Job
{
    std::map<std::string, Node> nodes; // perUserJobNode method
    // MeanCoreCons user_job_mcc;         // perUserJob method
    std::pair<double, int> mcc;        // <mcc, counts>
};

struct User
{
    // MeanCoreCons user_mcc;      // perUser method
    std::pair<double, int> mcc; // <mcc, counts>
    std::map<std::string, Node> nodes;
    std::map<std::string, Job> jobs;
};

struct MainMCCmap
{
    std::map<std::string, Node> nodes; // perNode method
    std::map<std::string, User> users;
};

int main()
{
    std::ifstream file("mean_core_user_job_node.csv");

    MainMCCmap mcc_map;

    CSVRow row;
    while (file >> row)
    {
        std::string user = row[1];
        std::string job = row[2];
        std::string node = row[3];
        int count = std::stoi(row[4]);
        double mcc = std::stod(row[5]);

        try{
            User u = mcc_map.users.at(user);
            // user found
            try{
                Job j = u.jobs.at(job);
                // job found
                try{
                    Node n = j.nodes.at(node);
                    // node found

                } catch (std::out_of_range){
                    j.nodes[node];
                    j.nodes[node].mcc = std::pair<double, int>(mcc, count);
                    // update also: perNode, perUser, perUserNode
                }

            } catch (std::out_of_range){
                // perUserJob
                u.jobs[job];
                u.jobs[job].mcc = std::pair<double, int>(mcc, count); 
                // perUserJobNode
                u.jobs[job].nodes[node];
                u.jobs[job].nodes[node].mcc = std::pair<double, int>(mcc, count); 
                // perUser
                double avg = (u.mcc.first*u.mcc.second + mcc*count) / (u.mcc.second + count);
                u.mcc.first = avg;
                u.mcc.second = u.mcc.second+count;
                // perNode
                try{
                    Node n = mcc_map.nodes.at(node);
                    // perNode if exist
                    double avg = (n.mcc.first*n.mcc.second + mcc*count) / (n.mcc.second + count);
                    n.mcc.first = avg;
                    n.mcc.second = n.mcc.second+count;
                } catch (std::out_of_range) {
                    // perNode if NOT exist
                    mcc_map.nodes[node];
                    mcc_map.nodes[node].mcc = std::pair<double, int>(mcc, count);
                }
            }

        } catch (std::out_of_range){
            // perUser
            mcc_map.users[user];
            mcc_map.users[user].mcc = std::pair<double, int>(mcc, count);
            // perUserNode
            mcc_map.users[user].nodes[node];
            mcc_map.users[user].nodes[node].mcc = std::pair<double, int>(mcc, count);
            // perUserJob
            mcc_map.users[user].jobs[job];
            mcc_map.users[user].jobs[job].mcc = std::pair<double, int>(mcc, count);
            // preUserJobNode
            mcc_map.users[user].jobs[job].nodes[node];
            mcc_map.users[user].jobs[job].nodes[node].mcc = std::pair<double, int>(mcc, count);

            try{
                Node n = mcc_map.nodes.at(node);
                // perNode if exist
                double avg = (n.mcc.first*n.mcc.second + mcc*count) / (n.mcc.second + count);
                n.mcc.first = avg;
                n.mcc.second = n.mcc.second+count;
            } catch (std::out_of_range) {
                // perNode if NOT exist
                mcc_map.nodes[node];
                mcc_map.nodes[node].mcc = std::pair<double, int>(mcc, count);
            }
        }

    }

    return 0;
}
