#!/usr/bin/env ruby
IO.foreach("files.txt") do |l|
  file=l.strip
  next if file.start_with? "<"
  p file
  `sed -i '.bak' 's|http://www.w3.org/2000/01/rdf-schema#||' #{file}          `
  `sed -i '.bak' 's|http://upload.wikimedia.org/wikipedia/commons/||' #{file} `
  `sed -i '.bak' 's|http://de.dbpedia.org/resource/||' #{file}                `
  `sed -i '.bak' 's|http://xmlns.com/foaf/0.1/||' #{file}                     `
  `sed -i '.bak' 's|http://dbpedia.org/ontology/||' #{file}                   `
  `sed -i '.bak' '|/1.1/rights>|d' #{file}                                    `
  `sed -i '.bak' 's|http://dbpedia.org/resource/||g' #{file}`
  `sed -i '.bak' 's|http://www.w3.org/1999/02/22-rdf-syntax-ns#||g' #{file}`
  `sed -i '.bak' 's|http://dbpedia.org/property/||g' #{file}`
  `sed -i '.bak' 's|<Category:|<|g' #{file}`
  `sed -i '.bak' 's|http://xmlns.com/foaf/0.1/||g' #{file}`
  `sed -i '.bak' 's|http://www.w3.org/2001/XMLSchema.||g' #{file}`
  `sed -i '.bak' 's|http://www.w3.org/2004/02/skos/core#||g' #{file}`
  `sed -i '.bak' 's|http://dbpedia.org/datatype/||g' #{file}`
  `sed -i '.bak' 's|http://www.georss.org/georss/||g' #{file}`
  `sed -i '.bak' 's|http://purl.org/dc/elements/1.1/||g' #{file}`
  `sed -i '.bak' 's/\\"/″/g' #{file}`
  # `sed -i '.bak' 's|http://www.w3.org/2003/01/geo/wgs84_pos#lat|latitude|g' #{file}` mappingbased_properties_cleaned_en ONLY ?
  # `sed -i '.bak' 's|http://www.w3.org/2003/01/geo/wgs84_pos#long|longitude|g' #{file}`
  # `sed -i '.bak' 's|http://www.opengis.net/gml/_Feature|Feature_Topic|g' #{file}` instance_types_en.ttl only
  # `sed -i '.bak' 's|http://schema.org/||g' #{file}`
  # `sed -i '.bak' 's|http://www.w3.org/2002/07/owl#||g' #{file}`
#`sed -i '.bak' 's|||g' #{file}`

# DELETIONS !!
  # `sed -i '.bak' '/__/d' #{file}` all
  `sed -i '.bak' '/__1/d' #{file}`
  `sed -i '.bak' '/__2/d' #{file}`
  `sed -i '.bak' '/__3/d' #{file}`
end

`sed -i '.bak' 's|http://purl.org/ontology/bibo/Book||g' instance_types_en.ttl`
`sed -i '.bak' 's|http://www.opengis.net/gml/_Feature|Feature_Topic|g' instance_types_en.ttl`
`sed -i '.bak' 's|http://schema.org/||g' instance_types_en.ttl`
`sed -i '.bak' 's|http://www.w3.org/2002/07/owl#||g' instance_types_en.ttl`
`sed -i '.bak' '/<Thing>/d' instance_types_en.ttl`
`sed -i '.bak' '/__/d' instance_types_en.ttl`

`sed -i '.bak' 's|http://www.w3.org/2003/01/geo/wgs84_pos#lat|latitude|g' mappingbased_properties_cleaned_en.ttl`
`sed -i '.bak' 's|http://www.w3.org/2003/01/geo/wgs84_pos#long|longitude|g' mappingbased_properties_cleaned_en.ttl`

`sed -i '.bak' 's|http://purl.org/dc/elements/1.1/||g' persondata_en.ttl`
`sed -i '.bak' '/http/d' persondata_en.ttl`

`sed -i '.bak' '/rights>/d' images_en.nt`

`sed -i '.bak' '/<Property>/d' raw_infobox_property_definitions_en.ttl`

# grep http raw_infobox_properties_en.ttl > raw_infobox_properties_en.urls.ttl
# `sed -i '.bak' '/http/d' raw_infobox_properties_en.ttl`
`grep '<thumbnail>' images_en.nt > thumbnails_en.nt`
`sed -i '.bak' '/<thumbnail>/d' images_en.nt`