insert into table1 (val) values ('a'), ('b'), ('c');
insert into table2 (val) values ('a'), ('b'), ('c');
checkpoint;
insert into table1 (val) values ('d'), ('e'), ('f');
insert into table2 (val) values ('d'), ('e'), ('f');
