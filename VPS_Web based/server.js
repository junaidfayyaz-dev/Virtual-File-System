const express = require("express");
const { spawn } = require("child_process");
const path = require("path");

const app = express();
app.use(express.json());
app.use(express.static("public"));

const vfsPath = path.join(__dirname, "vfs.exe");

// Start VFS
const vfs = spawn(vfsPath, [], {
    stdio: ["pipe", "pipe", "pipe"]
});

vfs.on("error", err => {
    console.error("Failed to start vfs.exe:", err);
});

// Buffer for collecting stdout
let stdoutBuffer = "";
let pendingResponse = null;

// Collect stdout
vfs.stdout.on("data", data => {
    stdoutBuffer += data.toString();

    // If we have a pending response and there's a newline, send it
    if (pendingResponse && stdoutBuffer.includes("\n")) {
        pendingResponse.send(stdoutBuffer);
        pendingResponse = null;
        stdoutBuffer = "";
    }
});

// Collect stderr as well
vfs.stderr.on("data", data => {
    stdoutBuffer += data.toString();
    if (pendingResponse) {
        pendingResponse.send(stdoutBuffer);
        pendingResponse = null;
        stdoutBuffer = "";
    }
});

app.post("/command", (req, res) => {
    const cmd = req.body.cmd;
    console.log("CMD:", cmd);

    if (pendingResponse) {
        return res.status(500).send("Previous command still processing");
    }

    pendingResponse = res;
    vfs.stdin.write(cmd + "\n");
});

app.listen(3000, () => {
    console.log("Server running at http://localhost:3000");
});
