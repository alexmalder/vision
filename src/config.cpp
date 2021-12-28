#include "vision.hpp"

Config::Config(string configPath)
{
    YAML::Node config = YAML::LoadFile(configPath);
    YAML::Node postgres_config = config["postgres"];
    YAML::Node create_tables = postgres_config["create"];
    YAML::Node prepared_statements = postgres_config["prepared"];
    for (auto create_table : create_tables) {
        this->create.push_back(create_table["query"].as<string>());
    }

    for (auto prepared_statement : prepared_statements) {
        this->prepared.push_back({ prepared_statement["name"].as<string>(),
                                   prepared_statement["query"].as<string>() });
    }
}