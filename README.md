Netbase - Semantic Graph Database 
------------

Netbase, your semantic web database.

We use it as Knowledge Graph containing over 600,000,000 nodes and statements, 
from all the Internet's finest sources, 
including [Freebase](http://www.freebase.com/), [WikiData](https://www.wikidata.org/wiki/Q54837), [DBPedia](http://wiki.dbpedia.org/), [Yago](https://en.wikipedia.org/wiki/YAGO_%28database%29), [Wordnet](https://en.wikipedia.org/wiki/WordNet), babelnet, ... 
as well as many custom GraphDBs. 

From simple entities to complex queries, this is by far the fastest db of its kind.
You can start *experimenting* with the API [right away](https://www.mashape.com/pannous/netbase), browse our [Knowledge Graph](http://netbase.pannous.com/html/verbose/example), run locally
or host as your own graph database, it is very quick to load.

You can start the console via `./netbase`
and get the available commands with :help

Netbase is much more than a hyperfast triple store: it has the [wordnet](http://wordnet.princeton.edu/) ontology and its relations built-in, so that you can quickly query with synonyms, transitive class hierarchies, part meronyms etc.

For example instead of asking about the "names of the daughters of Barack Obama" you could just search for "Obamas kids".

Extremely fast data import is provided for all common graph/data formats including n3, rdf, owl, csv, mysql, sql and xml;
The same formats are available for data access and data export together with the [blueprints](http://github.com/pannous/blueprints-netbase) standard.

This database is already used by millions of users through the [Jeannie](http://jeannie-assistant.com/) app.
