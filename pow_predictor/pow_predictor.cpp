#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <iterator>

using namespace std;

unordered_map<string, int> score1 {
    {"andy", 7}, {"al", 9}, {"bill", -3}, {"barbara", 12}
};


map<string, int> score2 {
    {"andy", 7}, {"al", 9}, {"bill", -3}, {"barbara", 12}
};

template<typename X, typename Y>
ostream& operator<<(ostream& os, const pair<X,Y>&p)
{
    return os << '{' << p.first << ',' << p.second << '}';
}

void user()
{
    cout<<"unordered: ";
    for(const auto& x:score1)
        cout << x << ", ";

    cout<<"\nordered: ";
    for(const auto& x:score2)
        cout << x << ", ";

    cout<<"\n";

    unordered_map<string, int>::iterator it = score1.begin();
    while(it != score1.end())
    {
        cout << pair<string, int>(it->first, it->second) << ", ";
        it++;
    }
    map<string, int>::iterator it2 = score2.begin();
}

int main()
{
    user();
    return 0;
}