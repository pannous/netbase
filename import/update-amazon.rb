#!/usr/bin/env ruby
mv amazon amazon.old
mkdir amazon
# `rsync --ignore-existing root@909.hqgmbh.eu:/datasearch1/amazon-direct/* .` #amazon 15G	total
`rsync --ignore-existing root@909.hqgmbh.eu:/datasearch1/amazon-direct/*.base.csv.gz amazon/` #1.6G  total

# softlinks
for file in `ls amazon/*2016*.base.csv.gz`.split("\n") do
  `cd amazon; ln -f -s #{file} #{file.sub(/_20\d+/,'')}`
end
