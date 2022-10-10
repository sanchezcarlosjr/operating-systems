const express = require('express');
const ws = require('ws');
const path = require('path');
const app = express();

app.get('/', function(req, res) {
  res.sendFile(path.join(__dirname, '/index.html'));
});

const wsServer = new ws.Server({ noServer: true });

wsServer.on('connection', socket => {
  console.log("CONNECTION");
  socket.on('message', message => console.log(message));
});


const server = app.listen(process.env.port || 3000);

server.on('upgrade', (request, socket, head) => {
  wsServer.handleUpgrade(request, socket, head, socket => {
    wsServer.emit('connection', socket, request);
  });
});
