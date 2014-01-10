package it.uniroma1.lcl.babelnet;

import it.uniroma1.lcl.jlt.util.Language;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import com.google.common.collect.HashMultimap;
import com.google.common.collect.Multimap;

import edu.mit.jwi.item.IPointer;
import edu.mit.jwi.item.POS;

/**
 *
 * A synset in {@link BabelNet}.
 *
 * @author navigli, ponzetto
 *
 */
public class BabelSynset implements Iterable<BabelSense>
{
	public static final String BABEL_SYNSET_PREFIX = "bn:";

	/*
	 * The senses that make up this BabelSynset
	 */
	private final List<BabelSense> senses;

	/*
	 * The region of BabelNet in which it lies
	 */
	private final BabelSynsetSource synsetSource;

	/*
	 * The list of WordNet offsets whose corresponding synsets it covers, if
	 * any
	 */
	private final List<String> wnOffsets;

	/*
	 * The id of the synset
	 */
	private final String id;

	/*
	 * The part-of-speech of the synset
	 */
	private final POS pos;

	/*
	 * A list of Strings encoding which sense item translates into which other,
	 * e.g. 1_4,7,9 means "sense #1 translates into senses #4, #7 and #9"
	 */
	private final List<String> translationMappings;

	/*
	 * The translation relations - which sense translates into which other -
	 * namely a type representation of the information encoded in
	 * #translationMappings
	 */
	private Multimap<BabelSense, BabelSense> translations;

	/*
	 * The main lemma of the synset
	 */
	private final String mainSense;

	/*
	 * The related synsets
	 */
	private Map<IPointer, List<BabelSynset>> relatedMap;

	/*
	 * The glosses of the synset
	 */
	private List<BabelGloss> glosses;

	/*
	 * The image URLs associated with the synset
	 */
	private List<BabelImage> images;

	/*
	 * The categories associated with the synset
	 */
	private List<BabelCategory> categories;

	/*
	 * The kind of synset
	 */
	private final BabelSynsetType synsetType;

	/**
	 * Creates a new instance of a {@link BabelSynset}.
	 *
	 * @param offset
	 * @param pos
	 * @param synsetSource
	 * @param wnOffsets
	 * @param senses
	 * @param translationMappings
	 * @param mainSense
	 */
	public BabelSynset(String offset, POS pos, BabelSynsetSource synsetSource,
					   List<String> wnOffsets, List<BabelSense> senses,
					   List<String> translationMappings,
					   List<BabelImage> images, List<BabelCategory> categories,
					   String mainSense, BabelSynsetType synsetType)
	{
		this.id = offset;
		this.pos = pos;
		this.synsetSource = synsetSource;
		if (wnOffsets.size() >= 1 && wnOffsets.get(0).isEmpty()) wnOffsets = new ArrayList<String>();
		this.wnOffsets = wnOffsets;
		this.senses = senses;
		this.images = images;
		this.categories = categories;
		this.translationMappings = translationMappings;
		this.mainSense = mainSense;
		this.synsetType = synsetType;
	}

	/**
	 * Gets the id of <b>this</b> {@link BabelSynset}.
	 *
	 * @return the id of <b>this</b> {@link BabelSynset}.
	 */
	public String getId() { return id; }

	/**
	 * Gets the part-of-speech of <b>this</b> {@link BabelSynset}.
	 *
	 * @return the part-of-speech of <b>this</b> {@link BabelSynset}.
	 */
	public POS getPOS() { return pos; }

	/**
	 * Gets the {@link BabelSynsetSource} of <b>this</b> {@link BabelSynset}.
	 *
	 * @return the {@link BabelSynsetSource} of <b>this</b> {@link BabelSynset}.
	 */
	public BabelSynsetSource getSynsetSource() { return synsetSource; }

	/**
	 * Gets the images ({@link BabelImage}s) of <b>this</b> {@link BabelSynset}.
	 *
	 * @return the images ({@link BabelImage}s) of <b>this</b>
	 *         {@link BabelSynset}.
	 */
	public List<BabelImage> getImages() { return new ArrayList<BabelImage>(images); }

	/**
	 * Gets the categories ({@link BabelCategory}s) of <b>this</b>
	 * {@link BabelSynset}.
	 *
	 * @return the categories ({@link BabelCategory}s) of <b>this</b>
	 *         {@link BabelSynset}.
	 */
	public List<BabelCategory> getCategories() { return new ArrayList<BabelCategory>(categories); }

	/**
	 * Gets the categories ({@link BabelCategory}s) of <b>this</b>
	 * {@link BabelSynset} in a specific language.
	 *
	 * @return the categories ({@link BabelCategory}s) of <b>this</b>
	 *         {@link BabelSynset} in a certain language.
	 */
	public List<BabelCategory> getCategories(Language... languages)
	{
		List<Language> langs = Arrays.asList(languages);
		List<BabelCategory> languageCategories = new ArrayList<BabelCategory>();

		for (BabelCategory category : categories)
			if (langs.contains(category.getLanguage()))
				languageCategories.add(category);

		return languageCategories;
	}

	/**
	 * Gets the WordNet offsets whose corresponding synsets <b>this</b>
	 * {@link BabelSynset} covers, if any.
	 *
	 * @return the WordNet offsets corresponding to <b>this</b>
	 *         {@link BabelSynset}.
	 */
	public List<String> getWordNetOffsets() { return new ArrayList<String>(wnOffsets); }

	/**
	 * Gets the "main sense" of a Babel synset: this is the stringified WordNet
	 * synset -- ie sense_1|sense_2|...sense_n, if the Babel synset overlaps
	 * with WordNet, or the first non-redirection page from Wikipedia,
	 * otherwise. If neither of these can be found, simply gives the lemma found
	 * in the synset.
	 *
	 * @return the main lemma of <b>this</b> Babel synset.
	 *
	 */
	public String getMainSense() { return mainSense; }

	/**
	 * Get the senses contained in <b>this</b> {@link BabelSynset}
	 *
	 * @return the senses of <b>this</b> Babel synset.
	 */
	public List<BabelSense> getSenses() { return new ArrayList<BabelSense>(senses); }

	/**
	 * Get the senses contained in <b>this</b> {@link BabelSynset} for an
	 * input language
	 *
	 * @return the senses of <b>this</b> Babel synset in a specific language.
	 */
	public List<BabelSense> getSenses(Language language)
	{
		List<BabelSense> languageSenses = new ArrayList<BabelSense>();

		for (BabelSense sense : senses)
			if (sense.getLanguage().equals(language))
				languageSenses.add(sense);

		return languageSenses;
	}

	/**
	 * Get the senses contained in <b>this</b> {@link BabelSynset} for an
	 * input sense source
	 *
	 * @return the senses of <b>this</b> Babel synset from an input sense source.
	 */
	public List<BabelSense> getSenses(BabelSenseSource source)
	{
		List<BabelSense> sourceSenses = new ArrayList<BabelSense>();
		for (BabelSense sense : senses)
			if (sense.getSource() == source)
				sourceSenses.add(sense);

		return sourceSenses;
	}

	/**
	 * Get the DBPedia URIs of the {@link BabelSense}s in a specific language
	 * found in <b>this</b> {@link BabelSynset}.
	 *
	 * @param languages
	 * @return the DBPedia URIs of the {@link BabelSense}s in this synset.
	 * @see BabelSense#getDBPediaURI()
	 */
	public List<String> getDBPediaURIs(Language... languages)
	{
		List<String> uris = new ArrayList<String>();
		List<Language> langs = Arrays.asList(languages);

		for (BabelSense sense : senses)
		{
			if (sense.getSource() != BabelSenseSource.WIKIRED
					&&
					(langs.isEmpty() || langs.contains(sense.getLanguage())))
			{
				String dBPediaURI = sense.getDBPediaURI();
				if (dBPediaURI != null) uris.add(dBPediaURI);
			}
		}

		return uris;
	}

	/**
	 * Gets the {@link BabelSense} for the input word in the given language
	 *
	 * @param language language of the sense
	 * @param lemma lemma of the sense
	 * @param sources possible sources for the sense
	 * @return the {@link BabelSense} for the input word in the given language
	 */
	public List<BabelSense> getSenses(Language language, String lemma, BabelSenseSource... sources)
	{
		List<BabelSenseSource> sourceList = sources.length == 0 ? null : Arrays.asList(sources);

		List<BabelSense> result = new ArrayList<BabelSense>();
		for (BabelSense sense : senses)
		{
			if (sense.getSimpleLemma().equalsIgnoreCase(lemma)
					&& sense.getLanguage().equals(language)
					&& (sourceList == null || sourceList.contains(sense.getSource())))
			{
				result.add(sense);
			}
		}
		return result;
	}

	/**
	 * Gets all translations between senses found in <b>this</b>
	 * {@link BabelSynset}.
	 *
	 * @return the map associating each {@link BabelSense} in <b>this</b> Babel
	 *         synset with its translations.
	 */
	public Multimap<BabelSense, BabelSense> getTranslations()
	{
		return getTranslations(0, 0.0);
	}

	/**
	 * Gets all translations between senses found in <b>this</b>
	 * {@link BabelSynset}.
	 *
	 * @param minSupport
	 *            the minimum number of sense-annotated examples needed for a
	 *            translation to be returned
	 * @param minConfidence
	 *            the minimum confidence score needed for a translation to be
	 *            returned
	 * @return the map associating each {@link BabelSense} in <b>this</b> Babel
	 *         synset with its translations.
	 */
	public Multimap<BabelSense, BabelSense> getTranslations(int minSupport, double minConfidence)
	{
		if (translations == null)
		{
			// init the translation relations
			translations = new HashMultimap<BabelSense, BabelSense>();
			for (String translationMapping : translationMappings)
			{
				String[] split = translationMapping.split("_");
				if (split.length != 2)
					throw new RuntimeException("Invalid translation mapping: " + translationMapping);

				Set<Integer> translationSetIds = new HashSet<Integer>();
				int src = Integer.valueOf(split[0]);
				translationSetIds.add(src);
				for (String targetIdx : split[1].split(","))
				{
					int target = Integer.valueOf(targetIdx);
					translationSetIds.add(target);
				}

				for (Integer elem1 : translationSetIds)
				{
					for (Integer elem2 : translationSetIds)
					{
						if (elem1 == elem2) continue;
						translations.put(senses.get(elem1), senses.get(elem2));
					}
				}
			}
		}

		// keeps only translations with a certain score
		Multimap<BabelSense, BabelSense> filteredTranslations =
			new HashMultimap<BabelSense, BabelSense>();

		for (BabelSense srcSense : translations.keySet())
		{
			for (BabelSense targetSense : translations.get(srcSense))
			{
				BabelSenseTranslationInfo transInfo =
					targetSense.getSenseTranslationInfo();

				if (
						// Wikipedia translation
						transInfo == null
					 ||
					 	// must have appropriate score
					(
						transInfo.getSampleSize() >= minSupport
					 &&
						transInfo.getConfidence() >= minConfidence
					)
				)
					filteredTranslations.put(srcSense, targetSense);
			}
		}

		return filteredTranslations;
	}

	/**
	 * Gets the type of <b>this</b> {@link BabelSynset}, namely whether it's
	 * an entity, a concept, etc.
	 *
	 * @return the kind of {@link BabelSynset}.
	 */
	public BabelSynsetType getSynsetType()
	{
		return synsetType;
	}

	/**
	 * Gives the BabelNet synset ID given an offset and a part-of-speech.
	 *
	 * @param offset
	 * @param pos
	 * @return a BabelNet synset ID for a given offset and part-of-speech.
	 */
	public static String getSynsetID(int offset, POS pos)
	{
		return BABEL_SYNSET_PREFIX+zeroFillOffset(offset)+pos.getTag();
	}

	/**
	 * Gives the synset offset from a BabelNet synset ID.
	 *
	 * @param synsetID
	 * @return a synset offset from a BabelNet synset ID.
	 */
	public static int getOffsetFromSynsetID(String synsetID)
	{
		return Integer.parseInt(getOffsetStringFromSynsetID(synsetID));
	}

	/**
 	 * Gives the synset offset string from a BabelNet synset ID.
 	 *
	 * @param synsetID
	 * @return a synset offset string from a BabelNet synset ID.
	 */
	public static String getOffsetStringFromSynsetID(String synsetID)
	{
		return synsetID.substring(BABEL_SYNSET_PREFIX.length(), synsetID.length()-1);
	}

	/**
	 * Takes an integer in the closed range [0,99999999] and converts it into an
	 * eight decimal digit zero-filled string. E.g., "1" becomes "00000001",
	 * "1234" becomes "00001234", and so on. This is used for the generation of
	 * Synset and Word ID numbers.
	 *
	 * @param offset
	 * @return eight decimal digit zero-filled string used to identify Babel synsets
	 */
	public static String zeroFillOffset(int offset)
	{
		checkOffset(offset);
		final StringBuilder sb = new StringBuilder(8);
		final String offsetStr = Integer.toString(offset);
		int numZeros = 8 - offsetStr.length();
		for (int i = 0; i < numZeros; i++) sb.append('0');
		sb.append(offsetStr);
		return sb.toString();
	}

	/**
	 * Test whether the input offset is a valid one.
	 *
	 * @param offset
	 */
    public static void checkOffset(int offset)
    {
        if (isIllegalOffset(offset))
        {
        	throw new IllegalArgumentException(
        		"'" + offset +
        		"' is not a valid offset; offsets must be in the closed range [0,99999999]");
        }
    }

    /**
     * @return <code>true</code> if the specified offset is in the closed
     * range [0, 99999999]; <code>false</code> otherwise.
     */
    public static boolean isIllegalOffset(int offset)
    {
        return offset < 0 || 99999999 < offset;
    }

    /**
	 * Prints the {@link BabelSense}s of <b>this</b> {@link BabelSynset} only
	 * for a specific set of languages.
	 *
	 * @param languages
	 * @return a stringified representation of <b>this</b> Babel synset using
	 *         only the senses in a specific set of languages
	 */
	public String toString(Language... languages)
	{
		StringBuffer sb = new StringBuffer("{");
		Set<Language> languageSet = new HashSet<Language>(Arrays.asList(languages));

		for (BabelSense sense : senses)
		{
			if (!languageSet.isEmpty() && !languageSet.contains(sense.getLanguage())) continue;

			if (sb.length() > 1) sb.append(",");
			sb.append(" ").append(sense.toString());
		}
		sb.append(" }");

		return sb.toString();
	}

	/**
	 * Collects <i>all</i> {@link BabelSynset}s related to <b>this</b>
	 * {@link BabelSynset}.
	 *
	 * @return the {@link BabelSynset}s related to <b>this</b>
	 *         {@link BabelSynset}
	 * @throws IOException
	 */
	public Map<IPointer, List<BabelSynset>> getRelatedMap() throws IOException
	{
		if (relatedMap == null)
		{
			// init the relation map
			BabelNet bn = BabelNet.getInstance();
			relatedMap = new HashMap<IPointer, List<BabelSynset>>();
			List<BabelNetGraphEdge> edges = bn.getSuccessorEdges(getId());
			for (BabelNetGraphEdge e : edges)
			{
				IPointer relation = e.getPointer();
				List<BabelSynset> synsetsForRelation = relatedMap.get(relation);
				if (synsetsForRelation == null)
				{
					synsetsForRelation = new ArrayList<BabelSynset>();
					relatedMap.put(relation, synsetsForRelation);
				}
				BabelSynset synset = bn.getSynsetFromId(e.getTarget());
				synsetsForRelation.add(synset);
			}
		}
		return relatedMap;
	}

	/**
	 * Collects <i>all</i> {@link BabelGloss}es in the given languages
	 * for <b>this</b> {@link BabelSynset}.
	 *
	 * @param lang the gloss language
	 * @return all glosses of <b>this</b> {@link BabelSynset} for the input language
	 *
	 * @throws IOException
	 *
	 */
	public List<BabelGloss> getGlosses(Language lang) throws IOException
	{
		List<BabelGloss> glosses = new ArrayList<BabelGloss>();

		for (BabelGloss g : getGlosses())
			if (g.getLanguage() == lang) glosses.add(g);

		return glosses;
	}

	/**
	 * Collects <i>all</i> {@link BabelGloss}es for <b>this</b>
	 * {@link BabelSynset}.
	 *
	 * @return all glosses of <b>this</b> {@link BabelSynset}
	 *
	 * @throws IOException
	 *
	 */
	public List<BabelGloss> getGlosses() throws IOException
	{
		if (glosses == null)
		{
			// init the glosses
			BabelNet bn = BabelNet.getInstance();
			this.glosses = bn.getGlosses(id);
		}
		return glosses;
	}

	@Override
	public String toString()
	{
		if (mainSense.isEmpty())
		{
			// the synset is not linked to WordNet or the English Wikipedia:
			// simply return the first Wikipage in the synset
			for (BabelSense sense : senses)
				if (sense.getSource() == BabelSenseSource.WIKI)
					return sense.getLemma();
			return mainSense;
		}
		return mainSense;
	}

//	@Override
	public Iterator<BabelSense> iterator()
	{
		return senses.iterator();
	}

	@Override
	public int hashCode()
	{
		return id.hashCode();
	}

	@Override
	public boolean equals(Object obj)
	{
		if(obj==null) return false;
		if(!(obj instanceof BabelSynset)) return false;

		BabelSynset other = (BabelSynset)obj;

		return other.id.equals(this.id);
	}
}
