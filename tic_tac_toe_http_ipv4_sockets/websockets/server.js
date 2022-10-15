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

const msgkey = 31337;
const queue = svmq.open(msgkey);
function pullNewMessages() {
	queue.pop({type: 2}, (err, data) => {
		if (err) throw err;
		const player = JSON.parse(data);
		console.log(player);
		database["players"][player.uuid].send(JSON.stringify(player));
		setImmediate(pullNewMessages);
	});
}


class GameController {
	save(socket) {
		const player = randomUUID();
		database["players"][player] = socket;
		database["players"]["count"]++;
		socket.send(JSON.stringify({"state": "12", uuid: player}));
		socket.on('message', (message) => {
			const response = JSON.parse(message);
			console.log(response);
			if(response.state === "1") {
				const game = randomUUID();
				database["games"][game] = [player];
				database["players_games"][response.uuid] = game;
				socket.send(JSON.stringify({state: "10", game}));
				return;
			}
			if(!!response.game && !database["games"][response.game]) {
				socket.send(JSON.stringify({state: "11"}));
				return;
			}
			if(response.state === "9") {
				database["games"][response.game].push(response.uuid);
				database["players_games"][response.uuid] = response.game;
				exec(`${path.join(__dirname, '/tictactoe.out')} X ${msgkey} ${database["games"][response.game][0]} O ${msgkey} ${database["games"][response.game][1]}`, (error, stdout, stderr) => {
					if (error) {
						console.error(`error: ${error.message}`);
						return;
					}
					if (stderr) {
						console.error(`stderr: ${stderr}`);
						return;
					}
					console.log(`DONE!`);
				});	
				console.log("NEW GAME");
				return;
			}
			if(response.state !== "2") {
				return;
			}
			queue.push(JSON.stringify(response), { type: 3 });
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
			const opponent = database["games"][game].filter(uuid => uuid !== player)[0];
			if (database["players"][opponent]) {
				database["players"][opponent].close();
				delete database["players"][opponent];
				delete database["players_games"][opponent];
				database["players"]["count"]--;
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

pullNewMessages();
