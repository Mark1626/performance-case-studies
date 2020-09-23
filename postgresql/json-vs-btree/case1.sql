--- Read 10,000 rows

explain analyse select k1.value as name, k2.value as description, t.field as field from tbl t
  join key_store k1 on t.name = k1.key
  join key_store k2 on t.tbl_desc = k2.key
  where k1.code = 'codeA' and k1.code = 'codeA' and t.id = 1000;

explain analyse select name -> 'codeA', tbl_desc -> 'codeA', field from tbl_json where id = 1000;

--- Write 10,000 rows

\copy tbl (name,tbl_desc,field) from './tbl_data.csv' DELIMITER ',' CSV HEADER;
\copy key_store (code,key,value) from './tbl_key_store_data.csv' DELIMITER ',' CSV HEADER;
\copy tbl_json (name,tbl_desc,field) from './tbl_json_data.csv' DELIMITER ';' CSV HEADER;

--- Read 20,000 rows

explain analyse select k1.value as name, k2.value as description, t.field as field from tbl t
  join key_store k1 on t.name = k1.key
  join key_store k2 on t.tbl_desc = k2.key
  where k1.code = 'codeA' and k1.code = 'codeA' and t.id = 1000;

explain analyse select name -> 'codeA', tbl_desc -> 'codeA', field from tbl_json where id = 1000;
