# VISION

## Motivation
We have a lot of cryptocurrency services, but this is stupidly. No linear conditions in you'r trading strategies...
Get tranparency of trading platform with computer vision technologies now!

## Workflow
-   user sign up and user sign in
-   show similarity by latest items with range
-   client-side configuration of search parameters
-   similar vector stabilization
-   service result callback
-   double-line chart
-   virtual contribution
-   debug visioner algorithm
-   auto purchases and auto sales//@file vision.h

### `vec_fill`
Model of operation
1. Find tuples by unix_val range
2. Insert founded values to a target array

Description
- `@param` source crypto data array pointer
- `@param` query real user input
- `@param` tuple_count tarantool tuple count
- `@param` target array for operation result
- `@return` int operation state

### `vec_slide`
Model of operation
1. Increment unix_val range by step size
2. Find real data with incremented unix_val range
3. Make vector stabilization with founded distance
4. Write vector to a database
5. Fill start_date and end_date values in result
6. Return result
 
Description
- `@param` cd crypto data array pointer
- `@param` result output example data structure
- `@param` tuple_count tarantool tuple count 
- `@param` target array for operation result
- `@return` int operation state

### `vec_similarity`
Model of operation
1. Search similarity vector(s) in source by target

Description
- `@param` a source array
- `@param` b target array
- `@param` end length of array
- `@return` double similarity

### `vec_distance`
Model of operation
1. Extract distance between two vectors

Description
- `@param` target array for distance extraction
- `@param` end length of array
- `@return` double 

### `vec_stabilization`
Model of operation
1. It's simple! Get median and up (or down) source vector by median
Description
- `@param` source array for stabilization by distance
- `@param` end length of array
- `@param` distance is factor
- `@return` int operation state

### `vec_merge`
Model of operation
1. This is not default vector merge. Need for multiple stabilization

Description:
- `@param` source array is read only
- `@param` target array is mutable
- `@param` end length of array
- `@return` int operation state

### `vec_search`
Model of operation
- this is high-level function
- used by zeromq or run manually for tests

Description
- `@param` query by user request
- `@param` result by system response
- `@return` int operation state