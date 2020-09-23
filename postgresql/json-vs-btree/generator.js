#!/usr/bin/env node
const fs = require("fs");
const fsPromises = fs.promises;

const ROWS_COUNT = 100_000;

const randomWord = (length) => {
  const chars =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  let word = "";
  for (let index = 0; index <= Math.floor(Math.random() * length); index++) {
    const idx = Math.floor(Math.random() * chars.length);
    word = word.concat(chars.charAt(idx));
  }
  return word;
};

const randomJSON = (len) =>
  `""{"""codeA""": """${randomWord(len)}""", """codeB""": """${randomWord(
    len
  )}"""}""`;

const randomKey = (idx) => `key_${randomWord(50)}_${idx}`;

const tbl_data = async (tblName, keyStoreName) => {
  fs.writeFileSync(tblName, "");
  fs.writeFileSync(keyStoreName, "");
  let dataFile = await fsPromises.open(tblName, "r+");
  let keyStore = await fsPromises.open(keyStoreName, "r+");

  dataFile.write(`name,tbl_desc,field\n`);
  keyStore.write(`code,key,value\n`);

  try {
    let promises = [];
    for (let index = 0; index < ROWS_COUNT; index++) {
      const nameKey = randomKey(index);
      const descKey = randomKey(ROWS_COUNT + index);
      promises.push(
        dataFile.write(`"${nameKey}","${descKey}","${randomWord(500)}"\n`)
      );
      promises.push(
        keyStore.write(`"codeA","${nameKey}","${randomWord(200)}"\n`)
      );
      promises.push(
        keyStore.write(`"codeB","${nameKey}","${randomWord(200)}"\n`)
      );
      promises.push(
        keyStore.write(`"codeA","${descKey}","${randomWord(200)}"\n`)
      );
      promises.push(
        keyStore.write(`"codeB","${descKey}","${randomWord(200)}"\n`)
      );
    }
    await Promise.all(promises);
  } finally {
    await dataFile.close();
    await keyStore.close();
  }
};

const tbl_json_data = async (tblName) => {
  fs.writeFileSync(tblName, "");
  let file = await fsPromises.open(tblName, "r+");
  file.write(`name,tbl_desc,field\n`);
  try {
    let promises = [];
    for (let index = 0; index < ROWS_COUNT; index++) {
      promises.push(
        file.write(`${randomJSON(200)};${randomJSON(200)};${randomWord(500)}\n`)
      );
    }
    await Promise.all(promises);
  } finally {
    await file.close();
  }
};

const tbl = "tbl_data.csv";
const keyStore = "tbl_key_store_data.csv";
const tblJSON = "tbl_json_data.csv";

const tbl2 = "tbl_data2.csv";
const keyStore2 = "tbl_key_store_data2.csv";
const tblJSON2 = "tbl_json_data2.csv";

Promise.all([
  tbl_json_data(tblJSON),
  tbl_data(tbl, keyStore),
  tbl_json_data(tblJSON2),
  tbl_data(tbl2, keyStore2),
])
  .then(() => console.log("files generated"))
  .catch((err) => console.log(err));
