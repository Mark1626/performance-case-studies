# Case study of jsonb vs regular join

[Results](./results.md)
[:arrow_backward:](../../README.md)

## About

#### GiST index and GIN index

GIN and GiST indexes are indexes that can be used to improve full text search.

GIN(Generalized Inverted Index) - Inverted Index

GiST(Generalized Search Tree) index is lossy

> **Trivia:** Lucene, Sphinx are based on Inverted indexes

#### Limitations

- JSONB column is not stored for `statistics`

### Usage

> **Note:** Indexes have not been added, they are left for the person running the code

- `make clean`
- `make populate`
- `psql -f queries.sql -d index_db`

### Future Works

- Try out different indexes on the JSONB column

### References

- https://www.postgresql.org/docs/12/textsearch-indexes.html
- 
