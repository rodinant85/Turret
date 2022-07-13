const express = require('express'),
    app = express(),
    http = require('http').createServer(app);

const host = '192.168.88.237';
const port = 8081;


app.use(express.static(__dirname));

// get count of active clients
app.get('/', (req, res) => {
    res.sendFile(`${__dirname}/src/server_files/index.html`);
});

http.listen(port, host, () => console.log(`Server listens http://${host}:${port}`));
