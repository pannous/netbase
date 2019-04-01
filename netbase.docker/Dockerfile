FROM ubuntu:18.04

MAINTAINER Quasiris <info@quasiris.com>

# RUN apt-get install -y apt-utils 
RUN apt-get update
RUN apt-get install -y libreadline7

# EXPOSE 6060 # use run -p 6060:6060

# Copy and run the install scripts.
# COPY *.sh /netbase/
# COPY netbase /netbase/
# COPY import/ /netbase/

# ADD * /root/
# COPY * /root/
COPY netbase /root/
COPY netbase.config /root/
COPY import/wins.n3 /root/import/
COPY .inputrc /root/
COPY .bash_aliases /root/

RUN ln -s /root/ /netbase
# RUN ln -s /netbase/ /root/

# ADD import/wins.n3 /netbase/import/wins.n3
# ADD /me/quasiris/DiAS/dias-feeding/pipelines/feeding/output/wins.n3 /netbase/import/wins.n3
# docker cp /me/quasiris/DiAS/dias-feeding/pipelines/feeding/output/wins.n3 netbase:/netbase/import/

RUN /netbase/netbase import wins.n3 exit 
RUN /netbase/netbase :server 
#-p 6060

# CMD /netbase/netbase server
# ENTRYPOINT ["redis", "-H", "something", "-u", "toto"]
# ENTRYPOINT ["netbase", "server"]