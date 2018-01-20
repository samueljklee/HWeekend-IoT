var http = require('http');

var server = http.createServer(function(req, res) {
  res.writeHead(200);
  res.end('Hello Http');
});
server.listen(3000, function () {
  console.log("== Server listening on port 3000");
});
