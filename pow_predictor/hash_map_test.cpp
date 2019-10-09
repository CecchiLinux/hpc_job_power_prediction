#include <iostream>
#include <map>

using namespace std;

struct MeanCoreCons
{
    pair<double, int> value; // <mcc, counts>
};

struct Node
{
    MeanCoreCons node_mcc; 
    pair<double, int> mcc; // <mcc, counts>
};

struct Job
{
    map<string, Node> nodes; // perUserJobNode method
    MeanCoreCons user_job_mcc; // perUserJob method
    pair<double, int> mcc; // <mcc, counts>
};

struct User
{
    MeanCoreCons user_mcc; // perUser method
    pair<double, int> mcc; // <mcc, counts>
    map<string, Node> nodes;
    map<string, Job> jobs;
};

struct MainMCCmap
{
    map<string, Node> nodes; // perNode method
    map<string, User> users;
};

template<typename X, typename Y>
ostream& operator<<(ostream& os, const pair<X, Y>&p)
{
    return os << '{' << p.first << ',' << p.second << '}';
}

int main()
{
    /* test var */
    string user1 = "a07smr01";
    string job1 = "A2REPO";
    string job2 = "job2";
    string node1 = "1";

    MainMCCmap mcc_values;

    // perNode method
    mcc_values.nodes["1"].mcc = pair<double, int>(6.02, 1);
    mcc_values.nodes["2"].mcc = pair<double, int>(7.02, 1);
    cout << mcc_values.nodes["1"].mcc << endl;

    // perUser method
    mcc_values.users[user1].mcc = pair<double, int>(6.02, 1);
    cout << mcc_values.users[user1].mcc << endl;

    // perUserNode method
    mcc_values.users[user1].nodes["1"].mcc = pair<double, int>(11.02, 2);
    mcc_values.users[user1].nodes["2"].mcc = pair<double, int>(13.03, 2);
    cout << mcc_values.users[user1].nodes["1"].mcc << endl;

    // perUserJob
    mcc_values.users[user1].jobs[job1].mcc = pair<double, int>(04.02, 2);
    cout << mcc_values.users[user1].jobs[job1].mcc << endl;

    // perUserJobNode
    mcc_values.users[user1].jobs[job1].nodes["1"].mcc = pair<double, int>(18.02, 2);
    cout << mcc_values.users[user1].jobs[job1].nodes["1"].mcc << endl;



    // v1

    if ( mcc_values.users.find(user1) == mcc_values.users.end() )
    {
        cout << "perNode" << endl; 
        cout << mcc_values.nodes[node1].mcc << endl;
    } else {
        if ( mcc_values.users[user1].jobs.find(job2) == mcc_values.users[user1].jobs.end() )
        {
            if ( mcc_values.users[user1].nodes.find(node1) == mcc_values.users[user1].nodes.end() )
            {
                cout << "perUser" << endl; 
                cout << mcc_values.users[user1].mcc << endl;
            } else {
                cout << "perUserNode" << endl; 
                cout << mcc_values.users[user1].nodes[node1].mcc << endl;
            }
        } else {
            if ( mcc_values.users[user1].jobs[job1].nodes.find(node1) == mcc_values.users[user1].jobs[job1].nodes.end() )
            {
                cout << "perUserJob" << endl; 
                cout << mcc_values.users[user1].jobs[job1].mcc << endl;
            } else {
                cout << "perUserJobNode" << endl; 
                cout << mcc_values.users[user1].jobs[job1].nodes[node1].mcc << endl;
            }

        }
    }

    // v2
    if ( mcc_values.users.count(user1))
    {
        cout << "trovato" << endl; 
    } else {
        cout << "non trovato" << endl; 
    }

    //v3
    try {
        User u = mcc_values.users.at(user1);
        try {
            Job j = u.jobs.at(job2);
            try {
                Node n = j.nodes.at(node1);
                cout << "perUserJobNode" << endl; 
                cout << n.mcc << endl;

            } catch(out_of_range){
                cout << "perUserJob" << endl; 
                cout << j.mcc << endl;
            }
        } catch(out_of_range){
            try {
                Node n = u.nodes.at(node1);
                cout << "perUserNode" << endl; 
                cout << n.mcc << endl;
            } catch(out_of_range){
                cout << "perUser" << endl; 
                cout << u.mcc << endl;
            }
        }
    }catch (out_of_range){
        try{
            Node n = mcc_values.nodes.at(node1);
            cout << "perNode" << endl; 
            cout << n.mcc << endl;
        } catch(out_of_range) {}
    }


    
    
    // v4
    // auto p_user = mcc_values.users.find(user1);
    // if (p_user != mcc_values.users.end())
    // {
    //     auto p_user_job = p_user->first.jobs.find(job1);
    //     if(p_user_job != mcc_values.users[user1].jobs.end())
    //     {

    //     }
    // }

    return 0;
}

