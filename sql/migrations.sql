---------------
-- DATA PART --
---------------

-- Create table data queue
CREATE TABLE data_queue (
    unix_val UInt64,
    datetime Date,
    symbol String,
    open Float64,
    high Float64,
    low Float64,
    close Float64,
    volume_original Float64,
    volume_usd Float64
) ENGINE = Kafka SETTINGS kafka_broker_list = '192.168.15.5:9092',
                            kafka_topic_list = 'data',
                            kafka_group_name = 'clickhouse_consumer_0',
                            kafka_format = 'JSONEachRow',
                            kafka_num_consumers = 1;
-- Create table result daily
CREATE TABLE data_daily (
    unix_val UInt64,
    datetime Date,
    symbol String,
    open Float64,
    high Float64,
    low Float64,
    close Float64,
    volume_original Float64,
    volume_usd Float64
) ENGINE = MergeTree() ORDER BY unix_val;

-- Create materialized view data consumer
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

-- Check data
SELECT * FROM data_daily;

-----------------
-- RESULT PART --
-----------------

-- Create table result queue
CREATE TABLE result_queue (
    unix_val UInt64,
    symbol String,
    open Float64,
    high Float64,
    low Float64,
    close Float64,
    volume_original Float64,
    volume_usd Float64,
    x UInt64,
    y UInt64,
    sim Float64
) ENGINE = Kafka SETTINGS kafka_broker_list = '192.168.15.5:9092',
                            kafka_topic_list = 'result',
                            kafka_group_name = 'clickhouse_consumer_0',
                            kafka_format = 'JSONEachRow',
                            kafka_num_consumers = 1;

-- Create table result daily
CREATE TABLE result_daily (
    unix_val UInt64,
    symbol String,
    open Float64,
    high Float64,
    low Float64,
    close Float64,
    volume_original Float64,
    volume_usd Float64,
    x UInt64,
    y UInt64,
    sim Float64
) ENGINE = MergeTree() ORDER BY unix_val;

-- Create materialized view result consumer
CREATE MATERIALIZED VIEW
	result_consumer 
TO 
	result_daily
AS SELECT * FROM
	result_queue 
GROUP BY 
	unix_val,
	symbol,
	open,
	high,
	low,
	close,
	volume_original,
	volume_usd,
    x,
    y,
    sim;