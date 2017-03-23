Netbase - Semantic Graph Database 
-----------

Netbase the Knowledge Graph of World Data, containing over 600000000 entities and statements from all the internet's finest sources, 
including [Freebase](http://www.freebase.com/), [WikiData](https://www.wikidata.org/wiki/Q54837), [DBPedia](http://wiki.dbpedia.org/), [Yago](https://en.wikipedia.org/wiki/YAGO_%28database%29), [Wordnet](https://en.wikipedia.org/wiki/WordNet), babelnet, ... 

From simple entities to complex queries, this is by far the fastest db of its kind.
You can start *experimenting* with the [API](https://www.mashape.com/pannous/netbase) in 
[Python](https://github.com/pannous/netbase-python),
[Ruby](https://github.com/pannous/netbase-ruby),
[Java](http://github.com/pannous/blueprints-netbase) (alpha) and
[Node.js](https://github.com/pannous/node-netbase),

*Browse* our [Knowledge Graph](http://netbase.pannous.com/html/verbose/example), run locally
or host as your own graph database, it is very quick to load.

Start the console via `./netbase`
and get the available commands with :help

Netbase is much more than a hyperfast triple store: it has the [wordnet](http://wordnet.princeton.edu/) ontology built-in.
Its relations are the back-bone of netbase, so that you can quickly query with synonyms, transitive class hierarchies, part meronyms etc.

Instead of asking about the "names of the daughters of Barack Obama" you could just search for "Obamas kids".

Extremely fast data import is provided for all common graph/data formats including n3, rdf, owl, csv, mysql, sql and xml;
The same formats are available for data access and data export together with the  standard.

This database is already used by millions of users through the [Jeannie](http://jeannie-assistant.com/) app.
Germans might also unknowingly had concert this technology through enterprise customers of [Quasiris Recharge](http://903.hqgmbh.eu/quasiris/wordpress/de/recharge/)
Here we show the power of natural language queries to get instant answers.
