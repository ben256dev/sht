module.exports = {
    apps: [{
        name: "sht",
        cwd: "/home/benjamin/sht",
        script: "go",
        args: "run ./backend",
        env: {
            ADDR: "127.0.0.1:8080"
        }
    }]
}

