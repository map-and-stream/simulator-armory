nc 127.0.0.1 3040 | hexdump -C

sudo lsof -i :3040

sudo lsof -t -i :3040 | xargs sudo kill -9

sudo lsof -i :2000

sudo lsof -t -i :2000 | xargs sudo kill -9
