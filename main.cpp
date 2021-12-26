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
    // init
    std::string configPath("config.yml");
    std::cout << "argc: " << argc << std::endl;
    Config *config = new Config(configPath);
    Repository *rep = new Repository(config);
    rep->init();
    // create service instance
    webserver ws = create_webserver(5000);
    SignUpAPI signUpAPI(rep);
    SignInAPI signInAPI(rep);
    CryptoAPI cryptoAPI(rep);
    ws.register_resource("/sign-in", &signInAPI);
    ws.register_resource("/sign-up", &signUpAPI);
    ws.register_resource("/crypto", &cryptoAPI);
    cout << "starting..." << endl;
    ws.start(true);
    return 0;
}
