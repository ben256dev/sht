module.exports = {
    apps: [{
        name: "sht",
        script: "/srv/sht/bin/sht-api",
        env: {
            ADDR: "127.0.0.1:8080",
            BLOB_DIR: "/b",
            SHL_RESOLVE: "/usr/local/bin/shl-resolve",
            PANDOC_BIN: "/usr/bin/pandoc"
        }
    }]
}
