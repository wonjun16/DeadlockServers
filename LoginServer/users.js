//const bcrypt = require('bcryptjs');
const db = require('./db');

const users = [];

// 회원 가입 함수
function register(email, password) {
  const existingUser = users.find(user => user.email === email);
  if (existingUser) {
    return null;
  }

  // 비밀번호 해시화
  const hashedPassword = bcrypt.hashSync(password, 10);
  const user = { email, password: hashedPassword };
  users.push(user);
  return user;
}

// 로그인 함수
//db에서 id pw 비교
function login(id, password, callback) {
  // 사용자 정보 조회 쿼리
  db.query('SELECT * FROM users WHERE id = ?', [id], (error, results) => {
    if (error) {
      console.error('Database error:', error);
      return callback(error, null);
    }

    if (results.length > 0) {
      const user = results[0];
      return callback(null, user);
      // // 비밀번호 비교
      // const isMatch = bcrypt.compareSync(password, user.password);
      // if (isMatch) {
      //   return callback(null, user); // 로그인 성공 시 사용자 정보 반환
      // }
    }

    // 로그인 실패 시 null 반환
    callback(null, null);
  });
}

module.exports = { register, login };
