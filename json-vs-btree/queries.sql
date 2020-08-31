explain analyse select k1.value as name, k2.value as description, t.field as field from tbl t
  join key_store k1 on t.name = k1.key
  join key_store k2 on t.tbl_desc = k2.key
  where k1.code = 'codeA' and k1.code = 'codeA' and t.id = 1000;

explain analyse select name -> 'codeA', tbl_desc -> 'codeA', field from tbl_json where id = 1000;

create index on key_store (key);

explain analyse select k1.value as name, k2.value as description, t.field as field from tbl t
  join key_store k1 on t.name = k1.key
  join key_store k2 on t.tbl_desc = k2.key
  where k1.code = 'codeA' and k1.code = 'codeA' and t.id = 1000;

create index on tbl_json using gin (name);
create index on tbl_json using gin (tbl_desc);