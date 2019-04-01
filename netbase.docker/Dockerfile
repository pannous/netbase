FROM ubuntu:18.04

MAINTAINER Quasiris <info@quasiris.com>

# Copy and run the install scripts.
COPY *.sh /netbase/
COPY netbase /netbase/
COPY import/ /netbase/

CMD ln -s /netbase/ ~/

# ADD import/wins.n3 /netbase/import/wins.n3
# ADD /me/quasiris/DiAS/dias-feeding/pipelines/feeding/output/wins.n3 /netbase/import/wins.n3
# docker cp /me/quasiris/DiAS/dias-feeding/pipelines/feeding/output/wins.n3 netbase:/netbase/import/

# RUN apt-get update
# RUN apt-get install -y apt-utils 
RUN apt-get install -y libreadline7

EXPOSE 6060

CMD netbase :import wins.n3
CMD netbase :server
