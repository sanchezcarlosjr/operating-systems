const express = require('express');
const ws = require('ws');
const path = require('path');
const app = express();
const { randomUUID } = require('crypto');

app.get('/', function(req, res) {
  res.sendFile(path.join(__dirname, '/index.html'));
});

const wsServer = new ws.Server({ noServer: true });

class GameController {
	constructor() {
		this.game = new Map();
		this.players = 0;
		this.lastGame = "";
	}
	save(socket) {
		this.players++;
		if(this.players % 2 == 0) {
			this.game.get(this.lastGame).push(socket);
			this.game.get(this.lastGame)[0].send(JSON.stringify({state: "7", uuid: "0", symbol: "X"}));
			this.game.get(this.lastGame)[1].send(JSON.stringify({state: "7", uuid: "1", symbol: "O"}));
			this.game.get(this.lastGame)[0].send(JSON.stringify({state: "2", id: "", symbol: ""}));
			this.game.get(this.lastGame)[1].send(JSON.stringify({state: "3"}));
			this.game.get(this.lastGame).forEach(socket => {
				socket.on('message', (message) => {
					const player = JSON.parse(message);
					if(player.state !== "2") {
						return;
					}
					const uuid = parseInt(player.uuid);
					this.game.get(this.lastGame)[1-uuid].send(JSON.stringify({state: "2", id: player.id, symbol: player.symbol}));
					this.game.get(this.lastGame)[uuid].send(JSON.stringify({state: "3"}));
				});
			});
		} else {
			this.lastGame = randomUUID();
			this.game.set(this.lastGame, [socket]);
			socket.send(JSON.stringify({state: "1"}));
		}
	}
}
const gameController = new GameController();

wsServer.on('connection', socket => {
  gameController.save(socket);
});


const server = app.listen(process.env.port || 3000);

server.on('upgrade', (request, socket, head) => {
  wsServer.handleUpgrade(request, socket, head, socket => {
    wsServer.emit('connection', socket, request);
  });
});
