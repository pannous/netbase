package it.uniroma1.lcl.babelnet;

import it.uniroma1.lcl.jlt.Configuration;
import it.uniroma1.lcl.jlt.util.Language;

/**
 * The category of a {@link BabelSynset}
 * 
 * @author ponzetto
 *
 */
public class BabelCategory
{
	public static final String BABEL_CATEGORY_PREFIX = "BNCAT:";
	
	private static final String WIKIPEDIA_URL_INFIX = "wikipedia.org/wiki/";
	
	/*
	 * The category itself, e.g. "Scientists who commited suicide"
	 */
	private final String category;
	
	/*
	 * The language of the category label, English, Italian, etc.
	 */
	private final Language language;

	/**
	 * Creates a new instance of a {@link BabelCategory}
	 * 
	 * @param category
	 * @param language
	 */
	public BabelCategory(String category, Language language)
	{
		this.category = category;
		this.language = language;
	}
	
	/**
	 * Gets the label of <b>this</b> {@link BabelCategory}.
	 * 
	 * @return the label of <b>this</b> {@link BabelCategory}.
	 */
	public String getCategory()
	{
		return category;
	}

	/**
	 * Gets the language of <b>this</b> {@link BabelCategory}.
	 * 
	 * @return the language of <b>this</b> {@link BabelCategory}.
	 */
	public Language getLanguage()
	{
		return language;
	}
	
	@Override
	public String toString()
	{
		return
			new StringBuffer().
				append(BABEL_CATEGORY_PREFIX).append(language).
				append(":").append(category).toString();
	}
	
	/**
	 * Gets the URI of the Wikipedia page <b>this</b> {@link BabelCategory}
	 * corresponds to. For instance
	 * "http://en.wikipedia.org/wiki/Category:Mathematicians_who_committed_suicide"
	 * 
	 * for the English category
	 * 
	 * "Mathematicians_who_committed_suicide"
	 * 
	 * and
	 * 
	 * "http://de.wikipedia.org/wiki/Kategorie:Mitglied_der_Royal_Society"
	 * 
	 * for the German category "Kategorie:Mitglied der Royal Society"
	 * 
	 * @return the Wikipedia URI for the page corresponding to <b>this</b>
	 *         {@link BabelCategory}.
	 */
	public String getWikipediaURI()
	{
		String languageCategoryName =
			Configuration.getInstance().getCategoryPrefix(language);
		return
			new StringBuffer().
				append("http://").append(language.toString().toLowerCase()).
				append(".").append(WIKIPEDIA_URL_INFIX).
				append(languageCategoryName).append(category).toString();
	}
	
	/**
	 * Creates a new instance of a {@link BabelCategory} from a string with
	 * format <language_id>:<category_label>, e.g. EN:Scientist_who_committed_suicide
	 * 
	 * @param categoryString
	 * @return an instance of a {@link BabelCategory} from an input string
	 */
	public static BabelCategory fromString(String categoryString)
	{
		int idx = categoryString.indexOf(":");
		if (idx == -1) return null;
		
		String category = categoryString.substring(idx+1);
		Language language = Language.valueOf(categoryString.substring(0, idx));
		
		return new BabelCategory(category, language);
	}
	
}
