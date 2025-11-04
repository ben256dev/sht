#!/usr/bin/env bash
set -euo pipefail
install -d -m 0755 /srv/sht/{bin,etc}
install -m 0644 openapi.yaml /srv/sht/etc/openapi.yaml
install -m 0644 etc/sht-api.env /srv/sht/etc/sht-api.env
go mod tidy
go build -o /srv/sht/bin/sht-api
install -m 0644 etc/sht-api.service /etc/systemd/system/sht-api.service
install -m 0644 etc/shthub.nginx.conf /etc/nginx/sites-available/shthub.conf
ln -sf /etc/nginx/sites-available/shthub.conf /etc/nginx/sites-enabled/shthub.conf
systemctl daemon-reload
systemctl enable sht-api
systemctl restart sht-api
nginx -t && systemctl reload nginx

