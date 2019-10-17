#include "mcc_utils.cpp"

using namespace std;

const string input_pow_csv = "convert_dict_copy.csv";
const string output_mcc = "mcc_map";


int main() {
    mcc_utils::MainMCCmap mcc_map;
    
    mcc_utils::create_map(input_pow_csv, output_mcc); // convert the csv file into an hashmap and serialize it (to mcc_map file)
    mcc_map = mcc_utils::loadMap(output_mcc); // load the mcc map

    /* 
     * test some known data
     */
    string user1 = "a07smr01";
    string job1 = "INIT";
    string job2 = "LM_PRE";
    string node1 = "35";
    string node2 = "38";

    // perNode method
    cout << mcc_map.perNode(node1).first << endl;
    // perUser method
    cout << mcc_map.perUser(user1).first << endl;
    // perUserNode method
    cout << mcc_map.perUserNode(user1, node1).first << endl;
    // perUserJob
    cout << mcc_map.perUserJob(user1, job1).first << endl;
    // perUserJobNode
    cout << mcc_map.perUserJobNode(user1, job2, node2).first << endl;

}