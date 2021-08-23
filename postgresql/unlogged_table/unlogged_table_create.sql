create table if not exists table1 (
  id serial primary key,
  val text
 );

 create unlogged table if not exists table2 (
  id serial primary key,
  val text
 );
