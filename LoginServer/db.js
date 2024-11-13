const mysql = require('mysql2');

require("dotenv").config();

const {
  MYSQL_HOST,
  MYSQL_USER,
  MYSQL_PW,
  MYSQL_DB,
} = process.env;

const conn = mysql.createConnection({
  host : MYSQL_HOST,
  user : MYSQL_USER,
  password : MYSQL_PW,
  database : MYSQL_DB
})

conn.connect((error) => {
  if (error) {
    console.error('MySQL Connect Failed:', error);
    return;
  }
  console.log('Connect with MySQL');
});

function query(sql, params, callback) {
  conn.query(sql, params, (error, results) => {
    if (error) {
      console.error('Query error :', error);
      callback(error, null);
    } else {
      callback(null, results);
    }
  });
}

module.exports = { query };

// const getTableData = async (tableName) => {
//     try {
//       const connection = await dbPool.getConnection();
//       const [rows, fields] = await connection.execute(`SELECT * FROM ${tableName};`);
//       connection.release();
//       return rows;
//     } catch (error) {
//       console.error(error);
//       return false;
//     }
//   }