# Testing to see if compiler makes a difference

## TLDR;

No significant performance difference

---

Tests are run using

```sql
pgbench -i
pgbench -c 10
```

## Clang

```sql
postgres=# select version();
                                                  version

---------------------------------------------------------------------------------------------
---------------
 PostgreSQL 12.4 on x86_64-pc-linux-gnu, compiled by clang version 7.0.1-8 (tags/RELEASE_701/
final), 64-bit
(1 row)
```

### Results

```
transaction type: <builtin: TPC-B (sort of)>
scaling factor: 1
query mode: simple
number of clients: 10
number of threads: 1
number of transactions per client: 10
number of transactions actually processed: 100/100
latency average = 70.360 ms
tps = 142.126289 (including connections establishing)
tps = 142.669793 (excluding connections establishing)
```

## GCC

```sql
perf=# select version();
                                        version
----------------------------------------------------------------------------------------
 PostgreSQL 12.4 on x86_64-pc-linux-gnu, compiled by gcc (Debian 8.3.0-6) 8.3.0, 64-bit
(1 row)
```

### Result

```
scaling factor: 1
query mode: simple
number of clients: 10
number of threads: 1
number of transactions per client: 10
number of transactions actually processed: 100/100
latency average = 70.379 ms
tps = 142.087980 (including connections establishing)
tps = 142.708699 (excluding connections establishing)
```
