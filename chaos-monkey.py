#!/usr/bin/env python3

from extensions import *
import urllib
words=read_lines("/data/words/english.txt")
words+=read_lines("/data/words/german.txt")

def ran():
	return (words[int(random()*len(words))]).replace(".","")

def ran(length=10001):
    return ''.join(unichr(randi(50000)) for i in range(length))

url="http://localhost:8080/ee/"
while 1:
	try:
		word=ran()+"+"+ran()+" "+ran()
		# word=ran()+ran()+"+"+ran()
		print(word)
		# exit()
		curl(url+urlencode({'q':word}))
		# curl(url+parse.quote(word))
	except:
	  pass 