var svmq = require('svmq');
var msg = svmq.msg;
var MSGMAX = svmq.MSGMAX; // max message data size (hardcoded)

// Open/create a queue with key 31337 and flags 950 (0666 | IPC_CREAT)
// Throws an error on failure
var id = msg.get(31337, 950);

// Push a string to the queue
msg.snd(id, Buffer.from(JSON.stringify({"row": 2, "column": 2})), 3, 0, (err) => {
  if (err) throw err;
});

var bufferSize = MSGMAX;
msg.rcv(id, MSGMAX, 0, 0, (err, data) => {
  if (err) throw err;
  console.log('Received data: ' + data.toString());
});
