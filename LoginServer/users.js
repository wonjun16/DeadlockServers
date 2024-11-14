const bcrypt = require('bcryptjs');
const db = require('./db');

// 회원 가입
function register(id, password, nickname, callback) {
  //id, nickname 중복체크
  db.query('SELECT * FROM users WHERE id = ? OR nickname = ?', [id, nickname], (error, results) => {
    if(error){
      console.error('Check exist id Database error: ', error);
      return callback(error, null);
    }

    //중복된 id or nickname이 존재
    if(results.length > 0){
      return callback(null, null);
    }
    else{
      const hashedPassword = bcrypt.hashSync(password, 10);

      // 새로운 사용자 데이터베이스에 저장
      db.query('INSERT INTO users (id, password, nickname) VALUES (?, ?, ?)', [id, hashedPassword, nickname], (error, results) => {
        if(error){
          console.error('Insert new user Database error: ', error);
          return callback(error, null);
        }
    
        return callback(null, results.affectedRows);
      });
    }
  });


}

// 로그인
function login(id, password, callback) {
  // 사용자 정보 조회 쿼리
  db.query('SELECT * FROM users WHERE id = ?', [id], (error, results) => {
    if (error) {
      console.error('Database error:', error);
      return callback(error, null);
    }

    if (results.length > 0) {
      const user = results[0];
      
      // 비밀번호 비교
      const isMatch = bcrypt.compareSync(password, user.password);
      if (isMatch) {
        return callback(null, user); // 로그인 성공 시 사용자 정보 반환
      }
    }

    // 로그인 실패 시 null 반환
    return callback(null, null);
  });
}

module.exports = { register, login };
