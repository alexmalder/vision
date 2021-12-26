#include "vision.hpp"

int test_bcrypt()
{
    string pwd = "test";
    string hash = BCrypt::generateHash(pwd);
    cout << BCrypt::validatePassword(pwd, hash) << endl;
    cout << BCrypt::validatePassword("test1", hash) << endl;
    return 0;
}

int main(int argc, char **argv)
{
    int resp = api();
    return resp;
}
