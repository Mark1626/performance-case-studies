\copy tbl (name,tbl_desc,field) from './tbl_data.csv' DELIMITER ',' CSV HEADER;
\copy key_store (code,key,value) from './tbl_key_store_data.csv' DELIMITER ',' CSV HEADER;
\copy tbl_json (name,tbl_desc,field) from './tbl_json_data.csv' DELIMITER ';' CSV HEADER;

