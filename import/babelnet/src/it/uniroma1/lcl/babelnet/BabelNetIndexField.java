package it.uniroma1.lcl.babelnet;

/**
 * Field used to index {@link BabelSynset}s and {@link BabelSense}s in the
 * Lucene index.
 * 
 * @author navigli, ponzetto
 *
 */
public enum BabelNetIndexField
{
	/**
	 * Id: \d{8}bn
	 */
	ID,
	
	/**
	 * WordNet, Wikipedia, Wordnet+Wikipedia
	 */
	SOURCE,
	
	/**
	 * connection to one or more WordNet offsets
	 */
	WORDNET_OFFSET,

	/**
	 * Main sense of a BabelSynset
	 */
	MAIN_SENSE,
	
	/**
	 * Part of speech
	 */
	POS,
	
	/**
	 * All lemmata of the BabelSynset (cased)
	 */
	LEMMA,

	/**
	 * All lemmata of the BabelSynset (normalized to lowercase)
	 */
	LEMMA_TOLOWERCASE,
	
	/**
	 * WordNet, Wikipedia, WordNet translation, Wikipedia translation
	 */
	LEMMA_SOURCE,
	
	/**
	 * Language of lemmas
	 */
	LEMMA_LANGUAGE,	
	
	/**
	 * Sensekeys for lemmas mapping to WordNet
	 */
	LEMMA_SENSEKEY,
	
	/**
	 * Concatenation of language+lemma for exact search
	 */
	LANGUAGE_LEMMA,
	
	/**
	 * Weights of translations
	 */
	LEMMA_WEIGHT,	
	
	/**
	 * A one-to-many relation between a term and its translations
	 */
	TRANSLATION_MAPPING,
	
	/**
	 * Relation with other synsets
	 */
	RELATION,
	
	/**
	 * Entities/concepts
	 */
	TYPE,
	
	/**
	 * Images
	 */
	IMAGE,
	
	/**
	 * Categories
	 */
	CATEGORY,
	
	/**
	 * Glosses
	 */
	GLOSS, 
	
	/**
	 * BabelNet version
	 */
	VERSION;
}
