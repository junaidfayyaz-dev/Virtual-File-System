function sendCommand(cmdType) {
    let cmd = "";

    if (cmdType === "format") cmd = "format";
    else if (cmdType === "list") cmd = "list";
    else if (cmdType === "diskusage") cmd = "diskusage";
    else if (cmdType === "help") cmd = "help";

    else if (cmdType === "create") {
        const name = document.getElementById("createName").value.trim();
        if (!name) return alert("Enter file name");
        cmd = `create ${name}`;
    }
    else if (cmdType === "write") {
        const name = document.getElementById("writeName").value.trim();
        const text = document.getElementById("writeText").value.trim();
        if (!name || !text) return alert("Missing input");
        cmd = `write ${name} "${text}"`;
    }
    else if (cmdType === "read") {
        const name = document.getElementById("readName").value.trim();
        if (!name) return alert("Enter file name");
        cmd = `read ${name}`;
    }
    else if (cmdType === "find") {
        const name = document.getElementById("findName").value.trim();
        if (!name) return alert("Enter search term");
        cmd = `find ${name}`;
    }
    else if (cmdType === "delete") {
        const name = document.getElementById("deleteName").value.trim();
        if (!name) return alert("Enter file name to delete");
        cmd = `delete ${name}`;
    }
    else if (cmdType === "rename") {
        const oldName = document.getElementById("renameOld").value.trim();
        const newName = document.getElementById("renameNew").value.trim();
        if (!oldName || !newName) return alert("Enter both old and new file names");
        cmd = `rename ${oldName} ${newName}`;
    }

    const outDiv = document.getElementById("out");
    outDiv.innerHTML += `<div class="cmd">$ ${cmd}</div>`;
    outDiv.scrollTop = outDiv.scrollHeight;

    fetch("/command", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ cmd })
    })
    .then(res => res.text())
    .then(data => {
        data.split("\n").forEach(line => {
            if (line.trim()) outDiv.innerHTML += `<div class="info">${line}</div>`;
        });
        outDiv.innerHTML += "<br>";
        outDiv.scrollTop = outDiv.scrollHeight;
    })
    .catch(err => {
        outDiv.innerHTML += `<div class="error">${err}</div>`;
    });
}

// Clear terminal output
function clearTerminal() {
    document.getElementById("out").innerHTML = "";
}

// Clear all input fields
function clearAllInputs() {
    document.getElementById("createName").value = "";
    document.getElementById("writeName").value = "";
    document.getElementById("writeText").value = "";
    document.getElementById("readName").value = "";
    document.getElementById("findName").value = "";
    document.getElementById("deleteName").value = "";
    document.getElementById("renameOld").value = "";
    document.getElementById("renameNew").value = "";
}
