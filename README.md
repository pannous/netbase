Netbase - Semantic Graph Database 
-----------

Netbase the Knowledge Graph of World Data, containing over 600000000 entities and statements from all the internet's finest sources, 
including [Freebase](https://en.wikipedia.org/wiki/Freebase), [WikiData](https://en.wikipedia.org/wiki/wikidata), [DBPedia](http://wiki.dbpedia.org/), [Yago](https://en.wikipedia.org/wiki/YAGO_%28database%29), [Wordnet](https://en.wikipedia.org/wiki/WordNet), babelnet, ... )

## Usage

#### Online Examples

*Browse* our [Knowledge Graph](http://netbase.pannous.com/html/verbose/example), run locally
or host as your own graph database, it is very quick to load.

#### API

From simple entities to complex queries, this is by far the fastest db of its kind.
You can start *experimenting* with the [API](https://www.mashape.com/pannous/netbase) in 
[Python](https://github.com/pannous/netbase-python),
[Ruby](https://github.com/pannous/netbase-ruby),
[Java](http://github.com/pannous/blueprints-netbase) (alpha) and
[Node.js](https://github.com/pannous/node-netbase),

## Installation

```bash
git clone https://github.com/pannous/netbase
cd netbase
./netbase :help
```

After you have imported some data (see ./import/ folder),
start the server via `./netbase :server`

## Summary

Netbase is much more than a hyperfast triple store: It is a semantic inference system which has the [wordnet](http://wordnet.princeton.edu/) ontology built-in. Semantic relations are the back-bone of netbase, so that you can quickly query with synonyms, transitive class hierarchies, part meronyms etc.

Instead of asking about the "names of the daughters of Barack Obama" you could just search for "Obamas kids".

Extremely fast data import is provided for all common graph/data formats including n3, rdf, owl, csv, mysql, sql and xml;
The same formats are available for data access and data export together with the [blueprints](https://github.com/pannous/blueprints-netbase) standard.

This database is already used by millions of users through the Genie/[Voice Actions](http://voice-actions.com/) app.
Germans might also have contact with this technology as enterprise customers of [Quasiris Recharge](https://www.quasiris.de/de/recharge/). Here we show the power of natural language queries to get instant answers.
