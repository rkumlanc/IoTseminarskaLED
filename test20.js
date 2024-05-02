import * as http from 'http';
import express from 'express';
import ip from 'ip';
import path from 'path';
import { join, dirname } from 'path';
import { fileURLToPath } from 'url';
import WebSocket, { WebSocketServer } from 'ws';
import { Serializer } from 'v8';

const __dirname = dirname(fileURLToPath(import.meta.url));

const app = express();

const hostname = '192.168.151.144';
const port = 80;

const server = http.createServer(app).listen(port);

const wss1 = new WebSocketServer({ port: 8888 }); // WebSocket server for browser clients
const wss2 = new WebSocketServer({ port: 8811 }); // WebSocket server for ESP32 modules

app.get('/', function (req, res) {
    res.sendFile(path.join(__dirname + '/test20.html'));
});

wss1.broadcast = function broadcast(data) {
    wss1.clients.forEach(function each(client) {
        if (client.readyState === WebSocket.OPEN) {
            client.send(data);
        }
    });
};

wss2.broadcast = function broadcast(data) {
    wss2.clients.forEach(function each(client) {
        if (client.readyState === WebSocket.OPEN) {
            client.send(data);
        }
    });
};

wss1.on('connection', function (ws, req) {
    ws.on('message', function (strMessage) {
        const msg = JSON.parse(strMessage);
        if (msg.tipSporočila === "LED" || msg.tipSporočila === "tipka") {
            funkcijaLED(msg, ws);
        }
    });
});

wss2.on('connection', function (ws, req) {
    ws.on('message', function (strMessage) {
        const msg = JSON.parse(strMessage);
        if (msg.tipSporočila === "LED" || msg.tipSporočila === "potenciometer" || msg.tipSporočila === "fotoupornik" || msg.tipSporočila === "tipka") {
            funkcijaLED(msg, ws);
        }
    });
});

function funkcijaLED(msg, ws) {
    if (msg.pin === "5" && msg.status === "pressed") {
        const ledMsg = JSON.stringify({ tipSporočila: "LED", pin: ["PIN22", "PIN23", "PIN12", "PIN13"], vrednost: 1 });
        wss2.broadcast(ledMsg);
    } else {
        wss1.broadcast(JSON.stringify(msg));
        wss2.broadcast(JSON.stringify(msg));
    }
}

console.log("Server is running at http://" + ip.address() + ":" + port);
