# node-netbase
node.js module for [Netbase](https://github.com/pannous/node-netbase): a semantic Graph Database with wordnet, wikidata, freebase, csv, xml, ... importer


## Installation
stable release:

`sudo npm install -g netbase`


install development head:

`sudo npm install -g git://github.com/pannous/node-netbase`

## Usage
```
    netbase = require('netbase'),
    show = netbase.show,
    query = netbase.query;
    
    netbase.import('wordnet')
    result=netbase.query('opposite of bad')
    show(result)
```
