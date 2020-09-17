# Summary

With 10,000 columns in the tbl these are the results

Notes:
- In the second read case `BTREE` index was added to `key` column `key_store` table
- In the second read case `GIN` index was added to `name` and `tbl_desc` column in `tbl_json` table
- JSON seems to have an advantage as the join is avoided
- In the JSON scenario we add 2 `GIN` indexes on the tbl, write performance needs to be tested
- But it may be balanced as the volume of data in `key_store` tbl is `4x` larger

### Read Performance

|                     | Standard | JSON   |
|---------------------|----------|--------|
| Without Index       |  68.717  | 17.309 |
| With Index          |  0.101   | 0.056  |

## Write Performance

- TODO

## Case 1 - No indexes

```sql
nimalanm@/tmp:index_db> explain analyse select k1.value as name, k2.value as description, t.field as field from tbl t
   join key_store k1 on t.name = k1.key
   join key_store k2 on t.tbl_desc = k2.key
   where k1.code = 'codeA' and k1.code = 'codeA' and t.id = 1000;
+------------------------------------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                                                           |
|------------------------------------------------------------------------------------------------------------------------------------------------------|
| Gather  (cost=7040.51..13364.24 rows=2 width=64) (actual time=40.888..68.682 rows=2 loops=1)                                                         |
|   Workers Planned: 2                                                                                                                                 |
|   Workers Launched: 2                                                                                                                                |
|   ->  Parallel Hash Join  (cost=6040.51..12364.04 rows=1 width=64) (actual time=53.827..62.416 rows=1 loops=3)                                       |
|         Hash Cond: (k1.key = t.name)                                                                                                                 |
|         ->  Parallel Seq Scan on key_store k1  (cost=0.00..6011.33 rows=83250 width=37) (actual time=0.028..18.223 rows=66667 loops=3)               |
|               Filter: (code = 'codeA'::text)                                                                                                         |
|               Rows Removed by Filter: 66667                                                                                                          |
|         ->  Parallel Hash  (cost=6040.50..6040.50 rows=1 width=65) (actual time=36.186..36.186 rows=1 loops=3)                                       |
|               Buckets: 1024  Batches: 1  Memory Usage: 40kB                                                                                          |
|               ->  Hash Join  (cost=8.32..6040.50 rows=1 width=65) (actual time=23.059..36.129 rows=1 loops=3)                                        |
|                     Hash Cond: (k2.key = t.tbl_desc)                                                                                                 |
|                     ->  Parallel Seq Scan on key_store k2  (cost=0.00..5594.67 rows=166667 width=37) (actual time=0.012..18.397 rows=133333 loops=3) |
|                     ->  Hash  (cost=8.31..8.31 rows=1 width=67) (actual time=0.042..0.042 rows=1 loops=3)                                            |
|                           Buckets: 1024  Batches: 1  Memory Usage: 9kB                                                                               |
|                           ->  Index Scan using tbl_pkey on tbl t  (cost=0.29..8.31 rows=1 width=67) (actual time=0.027..0.028 rows=1 loops=3)        |
|                                 Index Cond: (id = 1000)                                                                                              |
| Planning Time: 0.226 ms                                                                                                                              |
| Execution Time: 68.717 ms                                                                                                                            |
+------------------------------------------------------------------------------------------------------------------------------------------------------+
EXPLAIN
Time: 0.086s
```

---

JSON

```sql
nimalanm@/tmp:index_db> explain analyse select name -> 'codeA', tbl_desc -> 'codeA', field from tbl_json where id = 1000;
+-------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                              |
|-------------------------------------------------------------------------------------------------------------------------|
| Index Scan using tbl_json_pkey on tbl_json  (cost=0.29..8.32 rows=1 width=92) (actual time=0.046..0.046 rows=1 loops=1) |
|   Index Cond: (id = 1000)                                                                                               |
| Planning Time: 17.309 ms                                                                                                |
| Execution Time: 0.062 ms                                                                                                |
+-------------------------------------------------------------------------------------------------------------------------+
EXPLAIN
Time: 0.034s
```

---

## Case 2 - Index on key_store key column

```sql
nimalanm@/tmp:index_db> explain analyse select k1.value as name, k2.value as description, t.field as field from tbl t
   join key_store k1 on t.name = k1.key
   join key_store k2 on t.tbl_desc = k2.key
   where k1.code = 'codeA' and k1.code = 'codeA' and t.id = 1000;
+--------------------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                                           |
|--------------------------------------------------------------------------------------------------------------------------------------|
| Nested Loop  (cost=9.17..33.00 rows=2 width=64) (actual time=0.071..0.073 rows=2 loops=1)                                            |
|   ->  Nested Loop  (cost=4.73..20.65 rows=1 width=66) (actual time=0.042..0.043 rows=1 loops=1)                                      |
|         ->  Index Scan using tbl_pkey on tbl t  (cost=0.29..8.31 rows=1 width=67) (actual time=0.006..0.007 rows=1 loops=1)          |
|               Index Cond: (id = 1000)                                                                                                |
|         ->  Bitmap Heap Scan on key_store k1  (cost=4.44..12.33 rows=1 width=37) (actual time=0.033..0.033 rows=1 loops=1)           |
|               Recheck Cond: (key = t.name)                                                                                           |
|               Filter: (code = 'codeA'::text)                                                                                         |
|               Rows Removed by Filter: 1                                                                                              |
|               Heap Blocks: exact=2                                                                                                   |
|               ->  Bitmap Index Scan on key_store_key_idx  (cost=0.00..4.44 rows=2 width=0) (actual time=0.027..0.027 rows=2 loops=1) |
|                     Index Cond: (key = t.name)                                                                                       |
|   ->  Bitmap Heap Scan on key_store k2  (cost=4.44..12.33 rows=2 width=37) (actual time=0.027..0.027 rows=2 loops=1)                 |
|         Recheck Cond: (key = t.tbl_desc)                                                                                             |
|         Heap Blocks: exact=2                                                                                                         |
|         ->  Bitmap Index Scan on key_store_key_idx  (cost=0.00..4.44 rows=2 width=0) (actual time=0.025..0.025 rows=2 loops=1)       |
|               Index Cond: (key = t.tbl_desc)                                                                                         |
| Planning Time: 0.358 ms                                                                                                              |
| Execution Time: 0.101 ms                                                                                                             |
+--------------------------------------------------------------------------------------------------------------------------------------+
EXPLAIN
Time: 0.020s
```

---

JSON

```sql
nimalanm@/tmp:index_db> explain analyse select name -> 'codeA', tbl_desc -> 'codeA', field from tbl_json where id = 1000;
+-------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                              |
|-------------------------------------------------------------------------------------------------------------------------|
| Index Scan using tbl_json_pkey on tbl_json  (cost=0.29..8.32 rows=1 width=92) (actual time=0.034..0.035 rows=1 loops=1) |
|   Index Cond: (id = 1000)                                                                                               |
| Planning Time: 0.113 ms                                                                                                 |
| Execution Time: 0.056 ms                                                                                                |
+-------------------------------------------------------------------------------------------------------------------------+
EXPLAIN
Time: 0.017s
```