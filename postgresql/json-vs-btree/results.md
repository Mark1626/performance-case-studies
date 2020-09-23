# Summary

With 10,000 columns in the tbl these are the results

Notes:
- In the second read case `BTREE` index was added to `key` column `key_store` table
- In the second read case `GIN` index was added to `name` and `tbl_desc` column in `tbl_json` table
- JSON seems to have an advantage as the join is avoided
- In the JSON scenario we add 2 `GIN` indexes on the tbl, write performance needs to be tested
- But it may be balanced as the volume of data in `key_store` tbl is `4x` larger

### Stats

## Case 1 - Start with no indexes

### Statistics

|                             | Standard       |  JSONB    |
|-----------------------------|----------------|-----------|
| Read from 10,000 rows       |  ~68ms         |  ~0.05ms  |
| Index                       |   BTREE        |     GIN   |
| Read from 10,000 rows       |  ~0.15ms       |  ~0.04ms  |
| Write 10,000 rows           |  0.3s + 10s    |    4s     |

### Explain

#### Read from 10,000 rows


```sql
nimalanm@/tmp:index_db> explain analyse select k1.value as name, k2.value as description, t.field as field from tbl t
   join key_store k1 on t.name = k1.key
   join key_store k2 on t.tbl_desc = k2.key
   where k1.code = 'codeA' and k1.code = 'codeA' and t.id = 1000;

+------------------------------------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                                                           |
|------------------------------------------------------------------------------------------------------------------------------------------------------|
| Gather  (cost=7039.51..13362.31 rows=2 width=64) (actual time=66.072..68.088 rows=2 loops=1)                                                         |
|   Workers Planned: 2                                                                                                                                 |
|   Workers Launched: 2                                                                                                                                |
|   ->  Parallel Hash Join  (cost=6039.51..12362.11 rows=1 width=64) (actual time=52.725..61.342 rows=1 loops=3)                                       |
|         Hash Cond: (k1.key = t.name)                                                                                                                 |
|         ->  Parallel Seq Scan on key_store k1  (cost=0.00..6010.33 rows=83267 width=38) (actual time=0.018..18.649 rows=66666 loops=3)               |
|               Filter: (code = 'codeA'::text)                                                                                                         |
|               Rows Removed by Filter: 66667                                                                                                          |
|         ->  Parallel Hash  (cost=6039.50..6039.50 rows=1 width=65) (actual time=35.092..35.092 rows=1 loops=3)                                       |
|               Buckets: 1024  Batches: 1  Memory Usage: 40kB                                                                                          |
|               ->  Hash Join  (cost=8.32..6039.50 rows=1 width=65) (actual time=22.260..35.034 rows=1 loops=3)                                        |
|                     Hash Cond: (k2.key = t.tbl_desc)                                                                                                 |
|                     ->  Parallel Seq Scan on key_store k2  (cost=0.00..5593.67 rows=166667 width=38) (actual time=0.062..19.492 rows=133333 loops=3) |
|                     ->  Hash  (cost=8.31..8.31 rows=1 width=67) (actual time=0.031..0.031 rows=1 loops=3)                                            |
|                           Buckets: 1024  Batches: 1  Memory Usage: 9kB                                                                               |
|                           ->  Index Scan using tbl_pkey on tbl t  (cost=0.29..8.31 rows=1 width=67) (actual time=0.022..0.023 rows=1 loops=3)        |
|                                 Index Cond: (id = 1000)                                                                                              |
| Planning Time: 41.126 ms                                                                                                                             |
| Execution Time: 68.151 ms                                                                                                                            |
+------------------------------------------------------------------------------------------------------------------------------------------------------+
EXPLAIN
```

---

JSON

```sql
nimalanm@/tmp:index_db> explain analyse select name -> 'codeA', tbl_desc -> 'codeA', field from tbl_json where id = 1000;
+-------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                              |
|-------------------------------------------------------------------------------------------------------------------------|
| Index Scan using tbl_json_pkey on tbl_json  (cost=0.29..8.32 rows=1 width=92) (actual time=0.039..0.040 rows=1 loops=1) |
|   Index Cond: (id = 1000)                                                                                               |
| Planning Time: 8.737 ms                                                                                                 |
| Execution Time: 0.059 ms                                                                                                |
+-------------------------------------------------------------------------------------------------------------------------+
EXPLAIN
```

---

#### Index table

---

#### Read from 10,000 rows after index

```sql
nimalanm@/tmp:index_db> explain analyse select k1.value as name, k2.value as description, t.field as field from tbl t
   join key_store k1 on t.name = k1.key
   join key_store k2 on t.tbl_desc = k2.key
   where k1.code = 'codeA' and k1.code = 'codeA' and t.id = 1000;
+--------------------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                                           |
|--------------------------------------------------------------------------------------------------------------------------------------|
| Nested Loop  (cost=9.17..33.00 rows=2 width=64) (actual time=0.103..0.105 rows=2 loops=1)                                            |
|   ->  Nested Loop  (cost=4.73..20.65 rows=1 width=66) (actual time=0.062..0.064 rows=1 loops=1)                                      |
|         ->  Index Scan using tbl_pkey on tbl t  (cost=0.29..8.31 rows=1 width=67) (actual time=0.010..0.010 rows=1 loops=1)          |
|               Index Cond: (id = 1000)                                                                                                |
|         ->  Bitmap Heap Scan on key_store k1  (cost=4.44..12.33 rows=1 width=38) (actual time=0.048..0.049 rows=1 loops=1)           |
|               Recheck Cond: (key = t.name)                                                                                           |
|               Filter: (code = 'codeA'::text)                                                                                         |
|               Rows Removed by Filter: 1                                                                                              |
|               Heap Blocks: exact=1                                                                                                   |
|               ->  Bitmap Index Scan on key_store_key_idx  (cost=0.00..4.44 rows=2 width=0) (actual time=0.042..0.042 rows=2 loops=1) |
|                     Index Cond: (key = t.name)                                                                                       |
|   ->  Bitmap Heap Scan on key_store k2  (cost=4.44..12.33 rows=2 width=38) (actual time=0.038..0.038 rows=2 loops=1)                 |
|         Recheck Cond: (key = t.tbl_desc)                                                                                             |
|         Heap Blocks: exact=1                                                                                                         |
|         ->  Bitmap Index Scan on key_store_key_idx  (cost=0.00..4.44 rows=2 width=0) (actual time=0.037..0.037 rows=2 loops=1)       |
|               Index Cond: (key = t.tbl_desc)                                                                                         |
| Planning Time: 2.619 ms                                                                                                              |
| Execution Time: 0.154 ms                                                                                                             |
+--------------------------------------------------------------------------------------------------------------------------------------+
EXPLAIN
```

---

JSON

```sql
nimalanm@/tmp:index_db> explain analyse select name -> 'codeA', tbl_desc -> 'codeA', field from tbl_json where id = 1000;
+-------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                              |
|-------------------------------------------------------------------------------------------------------------------------|
| Index Scan using tbl_json_pkey on tbl_json  (cost=0.29..8.32 rows=1 width=92) (actual time=0.019..0.021 rows=1 loops=1) |
|   Index Cond: (id = 1000)                                                                                               |
| Planning Time: 0.075 ms                                                                                                 |
| Execution Time: 0.038 ms                                                                                                |
+-------------------------------------------------------------------------------------------------------------------------+
EXPLAIN
Time: 0.017s
```

---

#### Write data to table

```sql
nimalanm@/tmp:index_db> \copy tbl (name,tbl_desc,field) from './tbl_data.csv' DELIMITER ',' CSV HEADER;
Time: 0.365s

nimalanm@/tmp:index_db> \copy key_store (code,key,value) from './tbl_key_store_data.csv' DELIMITER ',' CSV HEADER;
Time: 10.356s (10 seconds), executed in: 10.356s (10 seconds)

nimalanm@/tmp:index_db> \copy tbl_json (name,tbl_desc,field) from './tbl_json_data.csv' DELIMITER ';' CSV HEADER;

Time: 4.231s (4 seconds), executed in: 4.231s (4 seconds)
n
```

#### Read from 20,000 rows with index

```sql
nimalanm@/tmp:index_db> explain analyse select k1.value as name, k2.value as description, t.field as field from tbl t
   join key_store k1 on t.name = k1.key
   join key_store k2 on t.tbl_desc = k2.key
   where k1.code = 'codeA' and k1.code = 'codeA' and t.id = 1000;
+--------------------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                                           |
|--------------------------------------------------------------------------------------------------------------------------------------|
| Nested Loop  (cost=9.33..69.25 rows=8 width=64) (actual time=0.117..0.169 rows=8 loops=1)                                            |
|   ->  Nested Loop  (cost=4.88..28.70 rows=2 width=66) (actual time=0.068..0.072 rows=2 loops=1)                                      |
|         ->  Index Scan using tbl_pkey on tbl t  (cost=0.42..8.44 rows=1 width=67) (actual time=0.011..0.012 rows=1 loops=1)          |
|               Index Cond: (id = 1000)                                                                                                |
|         ->  Bitmap Heap Scan on key_store k1  (cost=4.46..20.24 rows=2 width=38) (actual time=0.052..0.054 rows=2 loops=1)           |
|               Recheck Cond: (key = t.name)                                                                                           |
|               Filter: (code = 'codeA'::text)                                                                                         |
|               Rows Removed by Filter: 2                                                                                              |
|               Heap Blocks: exact=2                                                                                                   |
|               ->  Bitmap Index Scan on key_store_key_idx  (cost=0.00..4.46 rows=4 width=0) (actual time=0.045..0.045 rows=4 loops=1) |
|                     Index Cond: (key = t.name)                                                                                       |
|   ->  Bitmap Heap Scan on key_store k2  (cost=4.46..20.24 rows=4 width=38) (actual time=0.042..0.044 rows=4 loops=2)                 |
|         Recheck Cond: (key = t.tbl_desc)                                                                                             |
|         Heap Blocks: exact=4                                                                                                         |
|         ->  Bitmap Index Scan on key_store_key_idx  (cost=0.00..4.46 rows=4 width=0) (actual time=0.039..0.039 rows=4 loops=2)       |
|               Index Cond: (key = t.tbl_desc)                                                                                         |
| Planning Time: 0.725 ms                                                                                                              |
| Execution Time: 0.251 ms                                                                                                             |
+--------------------------------------------------------------------------------------------------------------------------------------+
EXPLAIN
```

---

JSON

```sql
nimalanm@/tmp:index_db> explain analyse select name -> 'codeA', tbl_desc -> 'codeA', field from tbl_json where id = 1000;
+-------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                              |
|-------------------------------------------------------------------------------------------------------------------------|
| Index Scan using tbl_json_pkey on tbl_json  (cost=0.42..8.45 rows=1 width=92) (actual time=0.020..0.021 rows=1 loops=1) |
|   Index Cond: (id = 1000)                                                                                               |
| Planning Time: 0.076 ms                                                                                                 |
| Execution Time: 0.038 ms                                                                                                |
+-------------------------------------------------------------------------------------------------------------------------+
EXPLAIN
```


### Case 2 - No index on table

|                             | Standard     |  JSONB   |
|-----------------------------|--------------|----------|
| Read from 10,000 rows       |  68.717ms    |  ~0.1ms  |
| Write 10,000 rows           |  0.4s + 2s   |   1.4s   |
| Read now from 20,000 rows   |  122.477ms   |  ~0.1ms  |


#### Write 10,000 rows

```sql
nimalanm@/tmp:index_db> \copy tbl (name,tbl_desc,field) from './tbl_data.csv' DELIMITER ',' CSV HEADER;
Time: 0.395s

nimalanm@/tmp:index_db> \copy key_store (code,key,value) from './tbl_key_store_data.csv' DELIMITER ',' CSV HEADER;
Time: 2.176s (2 seconds), executed in: 2.176s (2 seconds)

nimalanm@/tmp:index_db> \copy tbl_json (name,tbl_desc,field) from './tbl_json_data.csv' DELIMITER ';' CSV HEADER;
Time: 1.154s (a second), executed in: 1.154s (a second)
```

#### Read now from 20,000 rows

```sql
nimalanm@/tmp:index_db> explain analyse select k1.value as name, k2.value as description, t.field as field from tbl t
   join key_store k1 on t.name = k1.key
   join key_store k2 on t.tbl_desc = k2.key
   where k1.code = 'codeA' and k1.code = 'codeA' and t.id = 1000;

+-------------------------------------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                                                            |
|-------------------------------------------------------------------------------------------------------------------------------------------------------|
| Gather  (cost=13069.27..25710.41 rows=2 width=64) (actual time=118.944..121.688 rows=8 loops=1)                                                       |
|   Workers Planned: 2                                                                                                                                  |
|   Workers Launched: 2                                                                                                                                 |
|   ->  Parallel Hash Join  (cost=12069.27..24710.21 rows=1 width=64) (actual time=98.789..114.695 rows=3 loops=3)                                      |
|         Hash Cond: (k1.key = t.name)                                                                                                                  |
|         ->  Parallel Seq Scan on key_store k1  (cost=0.00..12019.14 rows=165812 width=38) (actual time=0.015..33.584 rows=133333 loops=3)             |
|               Filter: (code = 'codeA'::text)                                                                                                          |
|               Rows Removed by Filter: 133334                                                                                                          |
|         ->  Parallel Hash  (cost=12069.26..12069.26 rows=1 width=65) (actual time=66.578..66.578 rows=1 loops=3)                                      |
|               Buckets: 1024  Batches: 1  Memory Usage: 72kB                                                                                           |
|               ->  Hash Join  (cost=8.45..12069.26 rows=1 width=65) (actual time=33.044..66.510 rows=1 loops=3)                                        |
|                     Hash Cond: (k2.key = t.tbl_desc)                                                                                                  |
|                     ->  Parallel Seq Scan on key_store k2  (cost=0.00..11185.91 rows=333291 width=38) (actual time=0.012..33.930 rows=266667 loops=3) |
|                     ->  Hash  (cost=8.44..8.44 rows=1 width=67) (actual time=0.058..0.058 rows=1 loops=3)                                             |
|                           Buckets: 1024  Batches: 1  Memory Usage: 9kB                                                                                |
|                           ->  Index Scan using tbl_pkey on tbl t  (cost=0.42..8.44 rows=1 width=67) (actual time=0.032..0.033 rows=1 loops=3)         |
|                                 Index Cond: (id = 1000)                                                                                               |
| Planning Time: 0.317 ms                                                                                                                               |
| Execution Time: 121.725 ms                                                                                                                            |
+-------------------------------------------------------------------------------------------------------------------------------------------------------+
EXPLAIN

```
--- 
JSON

```sql
nimalanm@/tmp:index_db> explain analyse select name -> 'codeA', tbl_desc -> 'codeA', field from tbl_json where id = 1000;
+-------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                              |
|-------------------------------------------------------------------------------------------------------------------------|
| Index Scan using tbl_json_pkey on tbl_json  (cost=0.42..8.44 rows=1 width=92) (actual time=0.025..0.026 rows=1 loops=1) |
|   Index Cond: (id = 1000)                                                                                               |
| Planning Time: 0.050 ms                                                                                                 |
| Execution Time: 0.039 ms                                                                                                |
+-------------------------------------------------------------------------------------------------------------------------+
```

