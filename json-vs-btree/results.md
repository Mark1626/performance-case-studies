# Case 1 - No indexes

```sql
nimalanm@/tmp:index_db> explain analyse select k1.value as name, k2.value as description, t.field as field from tbl t
   join key_store k1 on t.name = k1.key
   join key_store k2 on t.tbl_desc = k2.key
   where k1.code = 'codeA' and k1.code = 'codeA';
+----------------------------------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                                                         |
|----------------------------------------------------------------------------------------------------------------------------------------------------|
| Gather  (cost=10684.78..36842.69 rows=195980 width=64) (actual time=83.829..155.294 rows=200000 loops=1)                                           |
|   Workers Planned: 2                                                                                                                               |
|   Workers Launched: 2                                                                                                                              |
|   ->  Parallel Hash Join  (cost=9684.78..16244.69 rows=81658 width=64) (actual time=79.360..129.541 rows=66667 loops=3)                            |
|         Hash Cond: (k2.key = t.tbl_desc)                                                                                                           |
|         ->  Parallel Seq Scan on key_store k2  (cost=0.00..5594.67 rows=166667 width=38) (actual time=0.010..14.213 rows=133333 loops=3)           |
|         ->  Parallel Hash  (cost=9170.62..9170.62 rows=41133 width=66) (actual time=79.000..79.000 rows=33333 loops=3)                             |
|               Buckets: 131072  Batches: 1  Memory Usage: 11168kB                                                                                   |
|               ->  Parallel Hash Join  (cost=2606.54..9170.62 rows=41133 width=66) (actual time=19.378..66.980 rows=33333 loops=3)                  |
|                     Hash Cond: (k1.key = t.name)                                                                                                   |
|                     ->  Parallel Seq Scan on key_store k1  (cost=0.00..6011.33 rows=82878 width=38) (actual time=0.040..21.595 rows=66667 loops=3) |
|                           Filter: (code = 'codeA'::text)                                                                                           |
|                           Rows Removed by Filter: 66667                                                                                            |
|                     ->  Parallel Hash  (cost=1871.24..1871.24 rows=58824 width=67) (actual time=19.076..19.076 rows=33333 loops=3)                 |
|                           Buckets: 131072  Batches: 1  Memory Usage: 11264kB                                                                       |
|                           ->  Parallel Seq Scan on tbl t  (cost=0.00..1871.24 rows=58824 width=67) (actual time=0.013..6.934 rows=33333 loops=3)   |
| Planning Time: 0.225 ms                                                                                                                            |
| Execution Time: 166.067 ms                                                                                                                         |
+----------------------------------------------------------------------------------------------------------------------------------------------------+
EXPLAIN
Time: 0.183s
nimalanm@/tmp:index_db> explain analyse select k1.value as name, k2.value as description, t.field as field from tbl t
   join key_store k1 on t.name = k1.key
   join key_store k2 on t.tbl_desc = k2.key
   where k1.code = 'codeA' and k1.code = 'codeA';
+----------------------------------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                                                         |
|----------------------------------------------------------------------------------------------------------------------------------------------------|
| Gather  (cost=10684.78..36842.69 rows=195980 width=64) (actual time=79.116..156.787 rows=200000 loops=1)                                           |
|   Workers Planned: 2                                                                                                                               |
|   Workers Launched: 2                                                                                                                              |
|   ->  Parallel Hash Join  (cost=9684.78..16244.69 rows=81658 width=64) (actual time=75.914..130.919 rows=66667 loops=3)                            |
|         Hash Cond: (k2.key = t.tbl_desc)                                                                                                           |
|         ->  Parallel Seq Scan on key_store k2  (cost=0.00..5594.67 rows=166667 width=38) (actual time=0.017..15.240 rows=133333 loops=3)           |
|         ->  Parallel Hash  (cost=9170.62..9170.62 rows=41133 width=66) (actual time=75.586..75.586 rows=33333 loops=3)                             |
|               Buckets: 131072  Batches: 1  Memory Usage: 11168kB                                                                                   |
|               ->  Parallel Hash Join  (cost=2606.54..9170.62 rows=41133 width=66) (actual time=15.510..63.241 rows=33333 loops=3)                  |
|                     Hash Cond: (k1.key = t.name)                                                                                                   |
|                     ->  Parallel Seq Scan on key_store k1  (cost=0.00..6011.33 rows=82878 width=38) (actual time=0.027..21.152 rows=66667 loops=3) |
|                           Filter: (code = 'codeA'::text)                                                                                           |
|                           Rows Removed by Filter: 66667                                                                                            |
|                     ->  Parallel Hash  (cost=1871.24..1871.24 rows=58824 width=67) (actual time=15.273..15.274 rows=33333 loops=3)                 |
|                           Buckets: 131072  Batches: 1  Memory Usage: 11296kB                                                                       |
|                           ->  Parallel Seq Scan on tbl t  (cost=0.00..1871.24 rows=58824 width=67) (actual time=0.013..5.841 rows=33333 loops=3)   |
| Planning Time: 0.222 ms                                                                                                                            |
| Execution Time: 168.191 ms                                                                                                                         |
+----------------------------------------------------------------------------------------------------------------------------------------------------+
EXPLAIN
Time: 0.185s
```

---

JSON

```sql
nimalanm@/tmp:index_db> explain analyse select name -> 'codeA', tbl_desc -> 'codeA', field from tbl_json;
+-----------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                      |
|-----------------------------------------------------------------------------------------------------------------|
| Seq Scan on tbl_json  (cost=0.00..3919.00 rows=100000 width=92) (actual time=0.038..55.289 rows=100000 loops=1) |
| Planning Time: 0.135 ms                                                                                         |
| Execution Time: 61.674 ms                                                                                       |
+-----------------------------------------------------------------------------------------------------------------+
EXPLAIN
Time: 0.082s
nimalanm@/tmp:index_db> explain analyse select name -> 'codeA', tbl_desc -> 'codeA', field from tbl_json where id = 1;
+-------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                              |
|-------------------------------------------------------------------------------------------------------------------------|
| Index Scan using tbl_json_pkey on tbl_json  (cost=0.29..8.32 rows=1 width=92) (actual time=0.031..0.032 rows=1 loops=1) |
|   Index Cond: (id = 1)                                                                                                  |
| Planning Time: 0.094 ms                                                                                                 |
| Execution Time: 0.047 ms                                                                                                |
+-------------------------------------------------------------------------------------------------------------------------+
EXPLAIN
Time: 0.017s
```

---

## Case 2 - Index on key_store key column

```sql
nimalanm@/tmp:index_db> explain analyse select k1.value as name, k2.value as description, t.field as field from tbl t
   join key_store k1 on t.name = k1.key
   join key_store k2 on t.tbl_desc = k2.key
   where k1.code = 'codeA' and k1.code = 'codeA';
+----------------------------------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                                                         |
|----------------------------------------------------------------------------------------------------------------------------------------------------|
| Gather  (cost=10684.78..36842.69 rows=195980 width=64) (actual time=86.655..158.044 rows=200000 loops=1)                                           |
|   Workers Planned: 2                                                                                                                               |
|   Workers Launched: 2                                                                                                                              |
|   ->  Parallel Hash Join  (cost=9684.78..16244.69 rows=81658 width=64) (actual time=81.667..132.256 rows=66667 loops=3)                            |
|         Hash Cond: (k2.key = t.tbl_desc)                                                                                                           |
|         ->  Parallel Seq Scan on key_store k2  (cost=0.00..5594.67 rows=166667 width=38) (actual time=0.011..14.414 rows=133333 loops=3)           |
|         ->  Parallel Hash  (cost=9170.62..9170.62 rows=41133 width=66) (actual time=81.209..81.209 rows=33333 loops=3)                             |
|               Buckets: 131072  Batches: 1  Memory Usage: 11200kB                                                                                   |
|               ->  Parallel Hash Join  (cost=2606.54..9170.62 rows=41133 width=66) (actual time=24.295..69.873 rows=33333 loops=3)                  |
|                     Hash Cond: (k1.key = t.name)                                                                                                   |
|                     ->  Parallel Seq Scan on key_store k1  (cost=0.00..6011.33 rows=82878 width=38) (actual time=0.043..21.231 rows=66667 loops=3) |
|                           Filter: (code = 'codeA'::text)                                                                                           |
|                           Rows Removed by Filter: 66667                                                                                            |
|                     ->  Parallel Hash  (cost=1871.24..1871.24 rows=58824 width=67) (actual time=23.961..23.961 rows=33333 loops=3)                 |
|                           Buckets: 131072  Batches: 1  Memory Usage: 11264kB                                                                       |
|                           ->  Parallel Seq Scan on tbl t  (cost=0.00..1871.24 rows=58824 width=67) (actual time=0.029..12.276 rows=33333 loops=3)  |
| Planning Time: 2.681 ms                                                                                                                            |
| Execution Time: 169.207 ms                                                                                                                         |
+----------------------------------------------------------------------------------------------------------------------------------------------------+
EXPLAIN
Time: 0.190s

nimalanm@/tmp:index_db> explain analyse select k1.value as name, k2.value as description, t.field as field from tbl t
   join key_store k1 on t.name = k1.key
   join key_store k2 on t.tbl_desc = k2.key
   where k1.code = 'codeA' and k1.code = 'codeA' and t.id = 1000;
+--------------------------------------------------------------------------------------------------------------------------------------+
| QUERY PLAN                                                                                                                           |
|--------------------------------------------------------------------------------------------------------------------------------------|
| Nested Loop  (cost=9.17..33.00 rows=2 width=64) (actual time=0.187..0.190 rows=2 loops=1)                                            |
|   ->  Nested Loop  (cost=4.73..20.65 rows=1 width=66) (actual time=0.141..0.142 rows=1 loops=1)                                      |
|         ->  Index Scan using tbl_pkey on tbl t  (cost=0.29..8.31 rows=1 width=67) (actual time=0.043..0.044 rows=1 loops=1)          |
|               Index Cond: (id = 1000)                                                                                                |
|         ->  Bitmap Heap Scan on key_store k1  (cost=4.44..12.33 rows=1 width=38) (actual time=0.094..0.094 rows=1 loops=1)           |
|               Recheck Cond: (key = t.name)                                                                                           |
|               Filter: (code = 'codeA'::text)                                                                                         |
|               Rows Removed by Filter: 1                                                                                              |
|               Heap Blocks: exact=1                                                                                                   |
|               ->  Bitmap Index Scan on key_store_key_idx  (cost=0.00..4.44 rows=2 width=0) (actual time=0.085..0.085 rows=2 loops=1) |
|                     Index Cond: (key = t.name)                                                                                       |
|   ->  Bitmap Heap Scan on key_store k2  (cost=4.44..12.33 rows=2 width=38) (actual time=0.043..0.044 rows=2 loops=1)                 |
|         Recheck Cond: (key = t.tbl_desc)                                                                                             |
|         Heap Blocks: exact=1                                                                                                         |
|         ->  Bitmap Index Scan on key_store_key_idx  (cost=0.00..4.44 rows=2 width=0) (actual time=0.042..0.042 rows=2 loops=1)       |
|               Index Cond: (key = t.tbl_desc)                                                                                         |
| Planning Time: 0.409 ms                                                                                                              |
| Execution Time: 0.310 ms                                                                                                             |
+--------------------------------------------------------------------------------------------------------------------------------------+
EXPLAIN
Time: 0.022s

```
