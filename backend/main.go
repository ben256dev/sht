package main

import (
    "bytes"
    "crypto/sha256"
    "encoding/hex"
    "io"
    "net/http"
    "os"
    "os/exec"
    "path/filepath"
    "regexp"
    "sort"
    "strings"
    "time"

    "github.com/go-chi/chi/v5"
)

var (
    addr       = getenv("ADDR", "127.0.0.1:8080")
    blobDir    = getenv("BLOB_DIR", "/b")
    resolveBin = getenv("SHL_RESOLVE", "/usr/local/bin/shl-resolve")
    pandocBin  = getenv("PANDOC_BIN", "/usr/bin/pandoc")
    reUser     = regexp.MustCompile(`^[A-Za-z0-9_]{1,32}$`)
    reAlias    = regexp.MustCompile(`^[A-Za-z0-9._-]{1,128}$`)
)

func getenv(k, d string) string {
    v := os.Getenv(k)
    if v == "" {
        return d
    }
    return v
}

func main() {
    r := chi.NewRouter()

    r.Get("/healthz", func(w http.ResponseWriter, r *http.Request) {
        w.WriteHeader(200)
        _, _ = w.Write([]byte("ok"))
    })

    r.Get("/", func(w http.ResponseWriter, r *http.Request) {
        if r.URL.Path != "/" {
            http.NotFound(w, r)
            return
        }
        http.ServeFile(w, r, "./backend/static/index.html")
    })

    r.Get("/style.css", func(w http.ResponseWriter, r *http.Request) {
        http.ServeFile(w, r, "./backend/static/style.css")
    })

    r.Route("/u/{user}", func(r chi.Router) {
        r.Get("/{alias}", getAlias)
        r.Head("/{alias}", headAlias)
    })

    s := &http.Server{
        Addr:              addr,
        Handler:           r,
        ReadHeaderTimeout: 5 * time.Second,
        IdleTimeout:       60 * time.Second,
    }

    if err := s.ListenAndServe(); err != nil {
        panic(err)
    }
}

func headAlias(w http.ResponseWriter, r *http.Request) {
    serveAlias(w, r, true)
}

func getAlias(w http.ResponseWriter, r *http.Request) {
    serveAlias(w, r, false)
}

func serveAlias(w http.ResponseWriter, r *http.Request, headOnly bool) {
    user := chi.URLParam(r, "user")
    alias := chi.URLParam(r, "alias")

    if !reUser.MatchString(user) || !reAlias.MatchString(alias) {
        http.Error(w, "bad path", 400)
        return
    }

    q := r.URL.Query()
    vraw := q.Has("raw")
    vhtml := q.Has("html") || !vraw

    if vraw && vhtml {
        u := *r.URL
        qq := u.Query()
        qq.Del("raw")
        u.RawQuery = encodePresence(qq)
        w.Header().Set("Location", u.String())
        w.WriteHeader(301)
        return
    }

    ver := strings.TrimSpace(q.Get("v"))
    dl := q.Has("download")

    p, err := resolvePath(user, alias, ver)
    if err != nil {
        http.Error(w, "not found", 404)
        return
    }

    et := etagFile(p)
    lm := fileMtime(p).UTC().Format(http.TimeFormat)

    w.Header().Set("ETag", et)
    w.Header().Set("Last-Modified", lm)
    w.Header().Set("Cache-Control", "public, max-age=60")

    if dl {
        w.Header().Set("Content-Disposition", `attachment; filename="`+alias+`"`)
    }

    if vraw {
        mt := sniff(p)
        w.Header().Set("Content-Type", mt)
        if headOnly {
            w.WriteHeader(200)
            return
        }
        f, err := os.Open(p)
        if err != nil {
            http.Error(w, "gone", 410)
            return
        }
        defer f.Close()
        http.ServeContent(w, r, "", fileMtime(p), f)
        return
    }

    if headOnly {
        w.Header().Set("Content-Type", "text/html; charset=utf-8")
        w.WriteHeader(200)
        return
    }

    src, err := os.ReadFile(p)
    if err != nil {
        http.Error(w, "gone", 410)
        return
    }

    html, err := runPandoc(src)
    if err != nil {
        http.Error(w, "render error", 500)
        return
    }

    w.Header().Set("Content-Type", "text/html; charset=utf-8")
    _, _ = w.Write(addCanonical(r))
    _, _ = w.Write(html)
}

func resolvePath(user, alias, ver string) (string, error) {
    cmd := exec.Command(resolveBin, "--path", blobDir, user, alias, ver)
    out := &bytes.Buffer{}
    cmd.Stdout = out
    cmd.Stderr = &bytes.Buffer{}
    if err := cmd.Run(); err != nil {
        return "", err
    }

    p := strings.TrimSpace(out.String())
    if !strings.HasPrefix(p, filepath.Clean(blobDir)+string(os.PathSeparator)) {
        return "", os.ErrNotExist
    }

    return p, nil
}

func runPandoc(src []byte) ([]byte, error) {
    c := exec.Command(pandocBin, "-f", "gfm", "-t", "html5", "--quiet")
    c.Stdin = bytes.NewReader(src)

    var out bytes.Buffer
    c.Stdout = &out

    var errb bytes.Buffer
    c.Stderr = &errb

    if err := c.Run(); err != nil {
        return nil, err
    }

    return out.Bytes(), nil
}

func addCanonical(r *http.Request) []byte {
    u := *r.URL
    q := u.Query()
    if !q.Has("html") && !q.Has("raw") {
        q.Add("html", "")
    }
    u.RawQuery = encodePresence(q)

    var b strings.Builder
    b.WriteString("<!doctype html><meta charset=\"utf-8\"><link rel=\"canonical\" href=\"")
    b.WriteString(u.String())
    b.WriteString("\">")

    return []byte(b.String())
}

func sniff(p string) string {
    f, err := os.Open(p)
    if err != nil {
        return "application/octet-stream"
    }
    defer f.Close()

    buf := make([]byte, 512)
    n, _ := f.Read(buf)

    return http.DetectContentType(buf[:n])
}

func fileMtime(p string) time.Time {
    st, err := os.Stat(p)
    if err != nil {
        return time.Now()
    }
    return st.ModTime()
}

func etagFile(p string) string {
    f, err := os.Open(p)
    if err != nil {
        return `W/"0"`
    }
    defer f.Close()

    h := sha256.New()
    _, _ = io.Copy(h, f)

    return `"` + hex.EncodeToString(h.Sum(nil)) + `"`
}

func encodePresence(q map[string][]string) string {
    keys := make([]string, 0, len(q))
    for k := range q {
        keys = append(keys, k)
    }

    sort.Strings(keys)

    parts := []string{}
    for _, k := range keys {
        if k == "v" {
            if vs, ok := q["v"]; ok && len(vs) > 0 {
                parts = append(parts, "v="+vs[0])
            }
            continue
        }
        parts = append(parts, k)
    }

    return strings.Join(parts, "&")
}

