package it.uniroma1.lcl.babelnet;

import com.sleepycat.persist.impl.SimpleFormat;
import it.uniroma1.lcl.jlt.util.Language;
import it.uniroma1.lcl.jlt.wordnet.WordNet;

import edu.mit.jwi.item.IWord;
import edu.mit.jwi.item.POS;

/**
 * A sense in {@link BabelNet}.
 *
 * @author navigli, ponzetto
 *
 */
public class BabelSense
{
	/*
	 * The prefix of DBPedia URIs
	 */
	private static String DBPEDIA_PREFIX = "http://DBpedia.org/resource/";

	/*
	 * The lemma for this sense
	 */
	private final String lemma;

	/*
	 * The simple lemma for this sense
	 */
	private final String simpleLemma;

	/*
	 * The source of this lemma: WordNet, Wikipedia, a translation, etc.
	 */
	private final BabelSenseSource source;

	/*
	 * The sensekey of the WordNet sense to which this sense corresponds,
	 * if any
	 *
	 */
	private final String sensekey;

	/*
	 * The offset of the WordNet sense to which this sense corresponds,
	 * if any
	 *
	 */
	private final String wordNetOffset;

	/*
	 * The position of the WordNet sense to which this sense corresponds,
	 *
	 */
	private final Integer position;

	/*
	 * The language of this sense
	 */
	private final Language language;

	/*
	 * The part-of-speech of this sense
	 */
	private final POS pos;

	/*
	 * The synset to which this sense belongs to
	 *
	 */
	private final BabelSynset synset;

	/*
	 * A string encoding information about a translated sense, example:
	 * "0.75000_3_4" means that we have a confidence of .75 as given by
	 * having translated the source sense 3 times out of 4 as <b>this</b>
	 * sense
	 */
	private final String translationInfo;

	/*
	 * A structured representation of translation-related information
	 */
	private BabelSenseTranslationInfo senseTranslationInfo;
    private boolean simpleFormat=true;

    /**
	 * Creates a new instance of a {@link BabelSense}
	 *
	 * @param language
	 * @param lemma
	 * @param pos
	 * @param source
	 * @param sensekey
	 * @param synset
	 */
	public BabelSense(Language language, String lemma, POS pos,
					  BabelSenseSource source, String sensekey,
					  String wordNetOffset, int position, String translationInfo, BabelSynset synset)
	{
		// sets the "simple" fields
		this.language = language;
		this.lemma = lemma;
		this.pos = pos;
		this.source = source;
		this.sensekey = sensekey;
		this.wordNetOffset = wordNetOffset;
		this.position = position;
		this.synset = synset;
		this.translationInfo = translationInfo;

		// sets the simpleLemma field
		int idx = lemma.indexOf("(");
		if (idx > 0)
		{
			if (lemma.charAt(idx - 1) == '_')
			{
				simpleLemma = lemma.substring(0, idx - 1);
			}
			else
			{
				// this is to handle malformed titles like e.g. "Comber(fish)"
				simpleLemma = lemma.substring(0, idx);
			}
		}
		else
		{
			simpleLemma = lemma;
		}
	}

	/**
	 * Gets the lemma of <b>this</b> {@link BabelSense}.
	 *
	 * @return the lemma of <b>this</b> {@link BabelSense}.
	 */
	public String getLemma() { return lemma; }

	/**
	 * Gets the <i>simple lemma</i> of <b>this</b> {@link BabelSense}.
	 * This corresponds to the lemma itself, if the lemma comes from WordNet,
	 * or the lemma possibly without sense labels (i.e. parenthesis) if it
	 * comes from Wikipedia
	 *
	 * @return the <i>simple lemma</i> of <b>this</b> {@link BabelSense}.
	 */
	public String getSimpleLemma() { return simpleLemma; }

	/**
	 * Gets the part of speech of <b>this</b> {@link BabelSense}.
	 *
	 * @return the part of speech of <b>this</b> {@link BabelSense}.
	 */
	public POS getPOS() { return pos; }

	/**
	 * Gets the {@link BabelSenseSource} of <b>this</b> {@link BabelSense}.
	 *
	 * @return the {@link BabelSenseSource} of <b>this</b> {@link BabelSense}.
	 */
	public BabelSenseSource getSource() { return source; }

	/**
	 * Gets the {@link Language} of <b>this</b> {@link BabelSense}
	 *
	 * @return the {@link Language} of <b>this</b> {@link BabelSense}.
	 */
	public Language getLanguage() { return language; }

	/**
	 * Gets the {@link BabelSynset} to which <b>this</b> {@link BabelSense}
	 * belongs to.
	 *
	 * @return the {@link BabelSynset} of <b>this</b> {@link BabelSense}.
	 */
	public BabelSynset getSynset() { return synset; }

	/**
	 * Gets the sensekey of the WordNet sense to which this {@link BabelSense}
	 * corresponds, if any.
	 *
	 * @return the sensekey of the WordNet sense corresponding to <b>this</b>
	 *         {@link BabelSense}.
	 */
	public String getSensekey() { return sensekey; }

	/**
	 * Gets the WordNet offset of the WordNet sense to which this {@link BabelSense}
	 * corresponds, if any.
	 *
	 * @return the offset of the WordNet sense corresponding to <b>this</b>
	 *         {@link BabelSense}.
	 */
	public String getWordNetOffset() { return wordNetOffset; }

	/**
	 * Gets the WordNet position of the WordNet sense to which this {@link BabelSense}
	 * corresponds, if any.
	 *
	 * @return the position of the WordNet sense corresponding to <b>this</b>
	 *         {@link BabelSense}.
	 */
	public Integer getPosition() { return position; }

	/**
	 * Gets translation-related information about this {@link BabelSense}.
	 *
	 * @return translation-related information about this {@link BabelSense}.
	 */
	public BabelSenseTranslationInfo getSenseTranslationInfo()
	{
		if (senseTranslationInfo == null)
		{
			// init the field
			this.senseTranslationInfo =
				BabelSenseTranslationInfo.fromString(translationInfo);
		}
		return senseTranslationInfo;
	}

	/**
	 * Get a link to the corresponding DBPedia URI (empty {@link String} if no
	 * interlinking is available or possible)
	 *
	 * @see <a href="http://wiki.dbpedia.org/Interlinking">DBPedia
	 *      interlinking</a>
	 * @return a link to the corresponding DBPedia URI.
	 */
	public String getDBPediaURI()
	{
		if(language.equals(Language.EN))
			switch (source)
			{
				case WIKI:
				case WIKIRED:
					return DBPEDIA_PREFIX + lemma;
				default:
					return null;
			}
		return null;
	}

	/**
	 * Gets a String-based representation of <b>this</b>
	 * {@link BabelSense} alternative to the "canonical" one obtained using
	 * {@link #toString()}. This corresponds to a diesis-like representation if
	 * the sense belongs to WordNet, e.g. "car#n#1" or "funk#n#3", or the page
	 * title (with no prefix) it if corresponds to a Wikipedia sense, otherwise
	 * the lemma if it is a translation
	 *
	 * @return a string representating <b>this</b> {@link BabelSense}.
	 */
	public String getSenseString()
	{
		switch (source)
		{
			case WN:
				WordNet wn = WordNet.getInstance();
				IWord wnSense = wn.getSenseFromSenseKey(sensekey);
				return wn.senseToString(wnSense);
			case WIKI:
			case WIKIRED:
			case WNTR:
			case WIKITR:
			case OMWN:
			case OMWIKI:
				return lemma;
			default:
				throw new RuntimeException("Unknown sense source: " + source);
		}
	}

	/**
	 * Gets the <i>sense number</i> of <b>this</b> {@link BabelSense}. This
	 * corresponds to the sense number found in WordNet, if the sense belongs
	 * to WordNet, 0 otherwise.
	 *
	 * @return the sense number of <b>this</b> {@link BabelSense}.
	 */
	public int getSenseNumber()
	{
		switch (source)
		{
			case WN:
				WordNet wn = WordNet.getInstance();
				IWord wnSense = wn.getSenseFromSenseKey(sensekey);
				return wn.getSenseNumber(wnSense);
			case WIKI:
			case WIKIRED:
			case WNTR:
			case WIKITR:
			case OMWN:
			case OMWIKI:
				return 0;
			default:
				throw new RuntimeException("Unknown sense source: " + source);
		}
	}

	@Override
	public String toString()
	{
        if(simpleFormat) return simpleLemma.replaceAll(".*#", "");

		StringBuffer sb = new StringBuffer();

		sb.append(source).
		   append(":").append(language).
		   append(":").append(lemma);

		BabelSenseTranslationInfo translationInfo = getSenseTranslationInfo();
		if (translationInfo != null) sb.append("_").append(translationInfo);

		return sb.toString();
	}

	@Override
	public int hashCode()
	{
		return this.getSynset().getId().hashCode() + getLemma().hashCode() + getSource().hashCode();
	}

	@Override
	public boolean equals(Object obj)
	{
		if(obj==null) return false;
		if(!(obj instanceof BabelSense)) return false;

		BabelSense other = (BabelSense)obj;
		return	this.getSynset().getId().equals(other.getSynset().getId()) &&
				this.getLemma().equals(other.getLemma()) &&
				this.getSource().equals(other.getSource()) &&
				this.getLanguage().equals(other.getLanguage()) &&
				(this.sensekey == null || this.sensekey.equals(other.sensekey));
	}
}
