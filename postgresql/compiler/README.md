# Complier test

[Results](./results.md){: .button}
[Back](./../README.md){: .button}

A test to see if changing the compiler makes postgresql faster

---

### Rerunning

- Build the docker images with `docker build -t bench:<compiler> -f Dockerfile_<compiler> .`
- Start the container with `docker run --rm -it bench:<compiler>`

### Note

The docker images are incomplete run the following inside the container

```sh
adduser postgres
mkdir /usr/local/pgsql/data
chown postgres /usr/local/pgsql/data
su - postgres
/usr/local/pgsql/bin/initdb -D /usr/local/pgsql/data
/usr/local/pgsql/bin/pg_ctl -D /usr/local/pgsql/data -l logfile start
/usr/local/pgsql/bin/createdb test
/usr/local/pgsql/bin/psql test
```

### Reference

- https://www.postgresql.org/docs/current/install-short.html
