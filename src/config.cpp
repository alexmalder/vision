#include "../vision.hpp"

Config::Config(string configPath)
{
    YAML::Node config = YAML::LoadFile(configPath);
    cout << config << endl;
    this->create_account = config["postgres"]["create_account"].as<string>();
    this->create_crypto = config["postgres"]["create_crypto"].as<string>();
    this->insert_account = config["postgres"]["insert_account"].as<string>();
    this->insert_crypto = config["postgres"]["insert_crypto"].as<string>();
    this->select_account = config["postgres"]["select_account"].as<string>();
    this->select_crypto = config["postgres"]["select_crypto"].as<string>();
}
