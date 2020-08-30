create table tbl (
  id serial primary key,
  name STRING_COLUMN,
  tbl_desc STRING_COLUMN,
  field STRING_COLUMN
);

create table key_store (
  id serial primary key,
  code STRING_COLUMN,
  key STRING_COLUMN,
  value STRING_COLUMN
);

--- 

create table tbl_json (
  id serial primary key,
  name JSON_COLUMN,
  tbl_desc JSON_COLUMN,
  field STRING_COLUMN
);
