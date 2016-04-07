#!/usr/bin/env ruby
# `rsync --ignore-existing root@909.hqgmbh.eu:/datasearch1/amazon-direct/* .` #amazon 15G	total
`rsync --ignore-existing root@909.hqgmbh.eu:/datasearch1/amazon-direct/*.base.csv.gz .` #1.6G  total
for file in `ls *2016*.base.csv.gz`.split("\n") do
  `ln -f -s #{file} #{file.sub(/_20\d+/,'')}`
end