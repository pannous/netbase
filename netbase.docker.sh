cp netbase netbase.docker/
docker build -t netbase netbase.docker/ &&
# docker run -it netbase bash
# docker run -d --name netbase netbase
# docker exec -it netbase /bin/bash # 'ssh' shell
docker run -it  -p 6060:6060 --restart unless-stopped -v ~/.bash_history:/root/.bash_history netbase bash 
