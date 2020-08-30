populate:
	createdb index_db
	node generator.js
	m4 -DSTRING_COLUMN=text -DJSON_COLUMN=jsonb create_schema.sql | psql -d index_db
	psql -f populate_schema.sql -d index_db

clean:
	dropdb index_db

.PHONY: populate clean
