#!/usr/bin/env ruby
# encoding: utf-8
h={}
i=200000;
IO.foreach("synsets.tsv") do |l|
  puts l.to_i.to_s+"\t"+ i.to_s# I+" "+l
  i=i+1
  # puts l.to_i%1000000
  # z=l.to_i%1000000
  # puts l if h[z]
  # h[z]=1
end
