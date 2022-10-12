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
	"games": [],
	"players": {count: 0}
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
		const game = Math.floor(database["players"]["count"]/2);
		if (game == database["games"].length) {
			database["games"].push([]);
		}
		database["games"][game].push(player);
		database["players"]["count"]++;
		socket.on('message', (message) => {
			const player = JSON.parse(message);
			console.log(player);
			if(player.state !== "2") {
				return;
			}
			queue.push(JSON.stringify(player), { type: 3 });
		});
		if (database["players"]["count"]%2===1) {
			return;
		}
		exec(`${path.join(__dirname, '/tictactoe.out')} X ${msgkey} ${database["games"][game][0]} O ${msgkey} ${database["games"][game][1]}`, (error, stdout, stderr) => {
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
