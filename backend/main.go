package main

import (
    "log"
    "net/http"
    "os"
)

func getenv(k, d string) string {
    v := os.Getenv(k)
    if v == "" {
        return d
    }
    return v
}

func main() {
    addr := getenv("ADDR", "127.0.0.1:8080")

    http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
        if r.URL.Path != "/" {
            http.NotFound(w, r)
            return
        }
        http.ServeFile(w, r, "./backend/static/index.html")
    })

    http.HandleFunc("/style.css", func(w http.ResponseWriter, r *http.Request) {
        http.ServeFile(w, r, "./backend/static/style.css")
    })

    log.Fatal(http.ListenAndServe(addr, nil))
}

