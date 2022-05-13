#include <NumCpp.hpp>
#include <NumCpp/Functions/min.hpp>

int main(void)
{
    nc::NdArray<float> a = { { 1, 2 } };
    nc::NdArray<float> b = { { 3, 4 } };
    nc::NdArray c = nc::append(a, b, nc::Axis::COL);
    std::cout << c << std::endl;
    std::cout << nc::min(a) << std::endl;
    return 0;
}
