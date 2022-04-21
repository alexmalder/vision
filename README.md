# Main Page {#mainpage}

# VISION

## Motivation

We have a lot of cryptocurrency services, but this is stupidly. No linear conditions in you'r trading strategies...

Get tranparency of trading platform with computer vision technologies now!

## Service result

### Workflow

-   QUEUE
    -   `symbol`: uint64_t
    -   `ssize`: uint64_t
    -   `slide`: uint64_t
    -   `distance`: double
    -   `x`: uint64_t
    -   `y`: uint64_t
    -   `similarity`: double
    -   `source`: double[]
    -   `target`: double[]

## Note

-   symbols map
    -   `BCH/USD`: 1
    -   `BTC/USD`: 2
    -   `ETH/USD`: 3
    -   `LTC/USD`: 4
    -   `XRP/USD`: 5

## Workflow

-   user sign up and user sign in
-   show similarity by latest items with range
-   client-side configuration of search parameters
-   similar vector stabilization
-   service result callback
-   double-line chart
-   virtual contribution
-   debug visioner algorithm
-   auto purchases and auto sales

## Clickhouse and kafka

## Create table data queue

```sql
CREATE TABLE data_queue (
    unix_val UInt64,
    datetime String,
    symbol String,
    open Float64,
    high Float64,
    low Float64,
    close Float64,
    volume_original Float64,
    volume_usd Float64
) ENGINE = Kafka('192.168.15.5:9092', 'data', 'consumer0', 'JSONEachRow');
```

## Create table data daily

```sql
CREATE TABLE data_daily (
    unix_val UInt64,
    datetime String,
    symbol String,
    open Float64,
    high Float64,
    low Float64,
    close Float64,
    volume_original Float64,
    volume_usd Float64
) ENGINE = MergeTree() ORDER BY unix_val;
```

## Create materialized view data consumer

```sql
CREATE MATERIALIZED VIEW
	data_consumer 
TO 
	data_daily
AS SELECT * FROM
	data_queue 
GROUP BY 
	unix_val, 
	datetime, 
	symbol, 
	open, 
	high, 
	low, 
	close,
	volume_original, 
	volume_usd;
```