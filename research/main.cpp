#include <clickhouse/client.h>

using namespace clickhouse;

int main()
{
    /// Initialize client connection.
    Client client(ClientOptions().SetHost("localhost"));

    /// Create a table.
    client.Execute(
        "CREATE TABLE IF NOT EXISTS test.numbers (id UInt64, name String) ENGINE = Memory");

    /// Insert some values.
    {
        Block block;

        auto id = std::make_shared<ColumnUInt64>();
        id->Append(1);
        id->Append(7);

        auto name = std::make_shared<ColumnString>();
        name->Append("one");
        name->Append("seven");

        block.AppendColumn("id", id);
        block.AppendColumn("name", name);

        client.Insert("test.numbers", block);
    }
}
