package it.uniroma1.lcl.babelnet;

/**
 * A kind of {@link BabelSynset} -- namely, entity or class. 
 * 
 * @author ponzetto
 *
 */
public enum BabelSynsetType
{ 
	NAMED_ENTITY { public String toString() { return "Named Entity"; }},

	CONCEPT { public String toString() { return "Concept"; }};
}
