package it.uniroma1.lcl.babelnet;

/**
 * A type of Babel synset, flagging in which region of the KB the synset lies.
 *
 * @author navigli, ponzetto
 * 
 */
public enum BabelSynsetSource
{
	/**
	 * Synset from Wikipedia "only"
	 */
	WIKI,
	
	/**
	 * Synset from WordNet "only"
	 */	
	WN,
	
	/**
	 * Synset at the intersection of WordNet and Wikipedia
	 */
	WIKIWN,
	
	
}
