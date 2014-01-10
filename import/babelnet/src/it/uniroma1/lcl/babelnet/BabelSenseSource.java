package it.uniroma1.lcl.babelnet;

/** 
 * Enumeration of the different sources for the BabelNet senses.
 * 
 * @author navigli, ponzetto
 *
 */
public enum BabelSenseSource
{ 
	/**
	 * Lexicalization from Wikipedia
	 */
	WIKI,

	/**
	 * Lexicalization from a Wikipedia redirection
	 */
	WIKIRED,
	
	/**
	 * Lexicalization from WordNet
	 */
	WN,
	
	/**
	 * Lexicalization from an automatic translation of a Wikipedia concept
	 */
	WIKITR,

	/**
	 * Lexicalization from an automatic translation of a WordNet concept
	 */
	WNTR,
	
	/**
	 * Lexicalization from Open Multilingual WordNet
	 */
	OMWN,
	
	/**
	 * Lexicalization from OmegaWiki
	 */
	OMWIKI,
	
	/**
	 * Lexicalization from Wiktionary
	 */
	WIKT
}
