#include <iostream>
#include <vector>

std::vector<int> iteration(std::vector<int> src, int start, int end)
{
    std::vector<int> result;
    for (int i = start; i < end; i++) {
        result.push_back(src[i]);
    }
    return result;
}

int main()
{
    std::vector<int> crypto_data{};
    std::vector<int> source{};
    for (int i = 0; i < 181; i++) {
        crypto_data.push_back(i);
    }
    for (int i = 0; i < 31; i++) {
        source.push_back(i);
    }
    uint64_t resolution = 10;
    uint64_t x = 0;
    uint64_t y = source.size();
    while (x < crypto_data.size()) {
        while (y < crypto_data.size()) {
            std::cout << "X: " << x << " Y: " << y << "\n";
            y += resolution, x += resolution;
            std::vector<int> target = iteration(crypto_data, x, y);
            std::cout << "[";
            for (auto item : target) {
                std::cout << item << ",";
            }
            std::cout << "]";
        }
        x += resolution;
    }
}
