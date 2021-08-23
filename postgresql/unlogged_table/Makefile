run:
	createdb test
	psql -f unlogged_table_create.sql
	psql -f unlogged_table_query.sql

clean:
	dropdb test

.PHONY: clean run
