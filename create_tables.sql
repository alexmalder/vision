--DROP TABLE crypto;
--DROP TABLE contribution;

CREATE TABLE IF NOT EXISTS crypto (
    `Unix` UInt64,
    `Datetime` String,
    `Symbol` UInt64,
    `Open` Float64,
    `High` Float64,
    `Low` Float64,
    `Close` Float64,
    `VolumeOriginal` Float64,
    `VolumeUsd` Float64
) ENGINE = MergeTree()
ORDER BY (Unix);

CREATE TABLE IF NOT EXISTS contribution (
    `UserID` UInt32,
    `RequestID` UInt32,
    `StartDate` UInt32,
    `EndDate` UInt32,
    `Resolution` Float64,
    `Threshold` Float64
) ENGINE = MergeTree()
ORDER BY (user_id, request_id);
