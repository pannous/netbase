cp netbase netbase.docker/
cp netbase.js
cp netbase.css
cp netbase.html
docker build -t netbase netbase.docker/ &&
# docker run -it netbase bash
# docker run -d --name netbase netbase
# docker exec -it netbase /bin/bash # 'ssh' shell
docker run -it  -p 8080:8080 --restart unless-stopped -v ~/.bash_history:/root/.bash_history netbase bash 
# docker run -it  --restart unless-stopped -v ~/.bash_history:/root/.bash_history netbase bash
