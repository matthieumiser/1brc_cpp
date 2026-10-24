#include <algorithm>
#include <cstddef>
#include<fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

const char* map_file(const char* fname, size_t& length);
int main() {
    string dataText;
    ifstream stationData("measurements.txt");
    unordered_map<string, vector<float> > dataMap;

    while (getline(stationData, dataText)) {
        string city = dataText.substr(0, dataText.find(";"));
        float temp = stof(dataText.substr(dataText.find(";") + 1));
        if (dataMap.find(city) == dataMap.end()) {
            dataMap.insert({city, { temp }});
        } else {
            dataMap.at(city).push_back(temp);
        }
    }

    vector<string> output;
    for ( auto item : dataMap ) {
        float min = *min_element(item.second.begin(), item.second.end());

        // not the true median, good enough for now
        size_t n = item.second.size() / 2;
        nth_element(item.second.begin(), item.second.begin() + n, item.second.end());
        float med = item.second[n];

        float max = *max_element(item.second.begin(), item.second.end());
        output.push_back(item.first + "=" + to_string(min) + "/" + to_string(med) + "/" + to_string(max));
    }
    sort(output.begin(), output.end()); 
    cout << "{";
    for ( auto i : output ) {
        cout << i << ", ";
    }
    cout << "}";

    return 0;
}
