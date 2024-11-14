const express = require('express');
const users = require('./users');
// const jwt = require('jsonwebtoken');

const app = express();
const PORT = 8080;
//const SECRET_KEY = 'secret_key';

app.use(express.json());

// 회원 가입
app.post('/register', (req, res) => {
  const { id, password, nickname } = req.body;

  users.register(id, password, nickname, (error, result) => {
    if(error){
      console.error('Register error: ', error);
      return res.status(500).json({message: 'Server error'});
    }

    if (result) {
      return res.status(201).json({ message: 'User registered successfully' });
    } else {
      return res.status(400).json({ message: 'User already exists' });
    }
  });
});

// 로그인
app.post('/login', (req, res) => {
  const { id, password } = req.body;

  users.login(id, password, (error, user) => {
    if (error) {
      console.error('Login error:', error);
      return res.status(500).json({ message: 'Server error' });
    }

    if (user) {
      return res.json({ message: 'Login successful'});
      // JWT 토큰 생성
      //const token = jwt.sign({ id: user.id }, SECRET_KEY, { expiresIn: '1h' });
      //res.json({ message: 'Login successful', token });
    } else {
      return res.status(401).json({ message: 'Invalid id or password' });
    }
  });
});

// 서버 시작
app.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
});
