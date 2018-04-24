#!/usr/bin/env python

from extensions import *
import urllib
words=read_lines("/data/words/english.txt")
words+=read_lines("/data/words/german.txt")

def ran():
	return (words[int(random()*len(words))]).replace(".","")

url="http://localhost:8080/ee/"
while 1:
	# word=ran()+" "+ran()+" "+ran()
	word=ran()+ran()+"+"+ran()
	print(word)
	# curl(url+urlencode({'q':word}))
	# curl(url+parse.quote(word))