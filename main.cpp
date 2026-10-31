#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// for mmap:
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;
using Map = unordered_map<string_view, vector<float>>;

void handle_error(const char* msg) {
    perror(msg); 
    exit(255);
}

int main() {
    int fd = open("measurements.txt", O_RDONLY);
    if (fd == -1) 
        handle_error("open");

    struct stat sb;
    if (fstat(fd, &sb) == -1)
        handle_error("fstat");

    size_t len = sb.st_size;
    const char* p = static_cast<const char*>(mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0u));
    string_view data = string_view(p, len);
    if (p == MAP_FAILED)
        handle_error("mmap");

    auto end = p + len;
    
    Map m;
    size_t start = 0;
    while (start < len) {
        size_t city_pos = data.find(';', start);
        size_t tmp_pos = data.find('\n', city_pos);
        // cout << data.substr(start, city_pos - start) << " | " << 
        //     data.substr(city_pos + 1, tmp_pos - city_pos - 1) << "\n";
        auto city = data.substr(start, city_pos - start); 
        auto tmp = stof(string(data.substr(city_pos + 1, tmp_pos - city_pos - 1)));
        
        m[city].push_back(tmp);
        start = tmp_pos + 1;
    }

    vector<string> output;
    for ( auto& [key, values] : m ) {
        float min = *min_element(values.begin(), values.end());

        // not the true median, good enough for now
        size_t n = values.size() / 2;
        nth_element(values.begin(), values.begin() + n, values.end());
        float med = values[n];

        float max = *max_element(values.begin(), values.end());
        output.push_back(string(key) + "=" + to_string(min) + "/" + to_string(med) + "/" + to_string(max));
    }
    sort(output.begin(), output.end()); 
    cout << "{";
    for ( auto i : output ) {
        cout << i << ", ";
    }
    cout << "}";

    return 0;
}
