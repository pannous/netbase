package it.uniroma1.lcl.babelnet;

import it.uniroma1.lcl.jlt.util.Language;

/**
 * A gloss in {@link BabelNet}.
 *
 * @author ponzetto
 *
 */
public class BabelGloss
{
	/*
	 * The region of BabelNet from which this gloss comes from
	 */
	private final BabelSenseSource source;

	/*
	 * The WordNet or Wikipedia sense from which the sense is taken
	 */
	private final String sourceSense;

	/*
	 * The language of the gloss
	 */
	private final Language language;

	/*
	 * The gloss itself
	 */
	private final String gloss;

	/**
	 * Creates a new instance of a {@link BabelGloss}
	 *
	 * @param source
	 * @param sourceSense
	 * @param language
	 * @param gloss
	 */
	public BabelGloss(BabelSenseSource source, String sourceSense,
					  Language language, String gloss)
	{
		this.source = source;
		this.sourceSense = sourceSense;
		this.language = language;
		this.gloss = gloss;
	}

	/**
	 * The region where the concept for <b>this</b> gloss lies.
	 *
	 * @return the region where the gloss concept lies
	 */
	public BabelSenseSource getSource()
	{
		return source;
	}

	/**
	 * The WordNet or Wikipedia sense from which <b>this</b> gloss is taken
	 *
	 * @return the correspondig sense for the gloss
	 */
	public String getSourceSense()
	{
		return sourceSense;
	}

	/**
	 * The language for <b>this</b> gloss
	 *
	 * @return the gloss language
	 */
	public Language getLanguage()
	{
		return language;
	}

	/**
	 * The actual gloss
	 *
	 * @return the gloss content as a String.
	 */
	public String getGloss()
	{
		return gloss;
	}
    public String toString() {
        return gloss;
    }

}
