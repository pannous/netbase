# wikidata-terms.de.nt
if [ $APPLE ] then
    alias zcat=gzcat
fi
zcat wikidata-terms.nt.gz| grep "@de ." | \
sed 's|http://www.wikidata.org/entity/||' | \
sed 's|http://www.w3.org/2000/01/rdf-schema#||' | \
sed 's|http://www.w3.org/2004/02/skos/core#||' | \
sed 's|http://schema.org/||' > wikidata-terms.de.nt

zcat wikidata-terms.nt.gz| grep "@en ." | \
sed 's|http://www.wikidata.org/entity/||' | \
sed 's|http://www.w3.org/2000/01/rdf-schema#||' | \
sed 's|http://www.w3.org/2004/02/skos/core#||' | \
sed 's|http://schema.org/||' > wikidata-terms.en.nt
