const express = require('express');
const ws = require('ws');
const path = require('path');
const app = express();
const { randomUUID } = require('crypto');
const svmq = require('svmq');
const { exec } = require('child_process');


app.get('/', function(req, res) {
  res.sendFile(path.join(__dirname, '/index.html'));
});

app.use(express.static(path.join(__dirname, 'public'))); 

const wsServer = new ws.Server({ noServer: true });

const database = {
	"games": {},
	"players": {count: 0},
	"players_games": {}
};

class QueueSvmq {
	constructor(msgkey) {
		this.queue = svmq.open(msgkey);
	}
	pull() {
		this.queue.pop({type: 2}, (err, data) => {
			if (err) {
				this.queue.close();
				return;
			}
			const player = JSON.parse(data);
			database["players"][player.uuid].send(JSON.stringify(player));
			setImmediate(this.pull.bind(this));
		});
	}
	close() {
		this.queue.close();
	}
	push(data) {
		this.queue.push(data, {type: 3});
	}
}

class GameController {
	constructor() {
		this.msgkey = 31337;
	}
	save(socket) {
		const player = randomUUID();
		database["players"][player] = socket;
		database["players"]["count"]++;
		socket.send(JSON.stringify({"state": "12", uuid: player}));
		socket.on('message', (message) => {
			const response = JSON.parse(message);
			if(response.state === "1") {
				const game = randomUUID();
				database["games"][game] = {};
				database["games"][game]["players"] = [player];
				database["players_games"][response.uuid] = game;
				socket.send(JSON.stringify({state: "10", game}));
				return;
			}
			if(!!response.game && !database["games"][response.game]) {
				socket.send(JSON.stringify({state: "11"}));
				return;
			}
			if(response.state === "9") {
				const queue = new QueueSvmq(this.msgkey);
				database["games"][response.game]["queue"] = queue;
				queue.pull();
				database["games"][response.game]["players"].push(response.uuid);
				const playerA = database["games"][response.game]["players"][0];
				const playerB = database["games"][response.game]["players"][1];
				database["players_games"][response.uuid] = response.game;
				exec(`${path.join(__dirname, '/tictactoe.out')} X ${this.msgkey} ${playerA} O ${this.msgkey} ${playerB}`, (error, stdout, stderr) => {
					if (error) {
						console.error(`error: ${error.message}`);
						return;
					}
					if (stderr) {
						console.error(`stderr: ${stderr}`);
						return;
					}
				});
				this.msgkey++;
				return;
			}
			if(response.state !== "2") {
				return;
			}
			const game = database["players_games"][response.uuid];
			database["games"][game]["queue"].push(JSON.stringify(response));
		});
		socket.on('close', () => {
			const game = database["players_games"][player];
			database["players"]["count"]--;
			if (!game) {
				delete database["players"][player];
				return;
			}
			delete database["players_games"][player];
			delete database["players"][player];
			if (!database["games"][game]) {
				return;
			}
			const opponent = database["games"][game]["players"].filter(uuid => uuid !== player)[0];
			if (database["players"][opponent]) {
				database["players"][opponent].close();
				delete database["players"][opponent];
				delete database["players_games"][opponent];
				database["players"]["count"]--;
			}
			if (database["games"][game]["queue"]) {
				database["games"][game]["queue"].close();
			}
			delete database["games"][game];
		});
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

