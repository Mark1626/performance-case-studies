#!/bin/bash

# Too lazy to create the user in Docker
adduser postgres
mkdir /usr/local/pgsql/data
chown postgres /usr/local/pgsql/data
su - postgres
/usr/local/pgsql/bin/initdb -D /usr/local/pgsql/data
/usr/local/pgsql/bin/pg_ctl -D /usr/local/pgsql/data -l logfile start
/usr/local/pgsql/bin/createdb perf
/usr/local/pgsql/bin/pgbench -i -d perf
/usr/local/pgsql/bin/pgbench -c 10
