var queue = require('svmq').open(31337);

queue.on('data', (data) => {
  // Data is provided as a Buffer. If you're passing Strings back and forth, be sure to use toString()
  // However, data does not have to be a String. It can be any type of data in Buffer form.
  console.log(data.toString());
});


