package it.uniroma1.lcl.babelnet;

import it.uniroma1.lcl.jlt.util.Language;

import java.util.Collections;
import java.util.Comparator;
import java.util.List;

/**
 * {@link Comparator} for {@link BabelSynset}s that (a) puts WordNet synsets
 * first; (b) sorts WordNet synsets based on the sense number of a specific
 * input word (see the constructor); (c) sorts Wikipedia synsets
 * lexicographically based on their main sense
 * 
 * @author navigli
 *
 */
public class BabelSynsetComparator implements Comparator<BabelSynset>
{
	/**
	 * The lemma used to sort synsets
	 */
	private final String lemma;
	
	/**
	 * Language used to sort synsets
	 */
	private Language language;
	
	/**
	 * Creates a new instance of a {@link BabelSynsetComparator} with sorting on the English language
	 * 
	 * @param word
	 *            the word whose sense numbers are used to sort the
	 *            {@link BabelSynset}s corresponding to WordNet synsets
	 */
	public BabelSynsetComparator(String word)
	{
		this(word, Language.EN);
	}
	 
	/**
	 * Creates a new instance of a {@link BabelSynsetComparator}
	 * 
	 * @param word
	 *            the word whose sense numbers are used to sort the
	 *            {@link BabelSynset}s corresponding to WordNet synsets
	 * @param language
	 *            the language used to sort senses 
	 */
	public BabelSynsetComparator(String word, Language language)
	{
		this.lemma =  word;
		this.language = language;
	}

	@Override
    public int compare(BabelSynset b1, BabelSynset b2)
    {
    	BabelSynsetSource b1Source = b1.getSynsetSource();
    	BabelSynsetSource b2Source = b2.getSynsetSource();

        boolean bWordNet1 =
        	b1Source == BabelSynsetSource.WN ||
        	b1Source == BabelSynsetSource.WIKIWN;
        bWordNet1 &= !b1.getSenses(Language.EN, lemma, BabelSenseSource.WN).isEmpty();
        
        boolean bWordNet2 =
        	b2Source == BabelSynsetSource.WN ||
        	b2Source == BabelSynsetSource.WIKIWN;
        bWordNet2 &= !b2.getSenses(Language.EN, lemma, BabelSenseSource.WN).isEmpty();

        // if the two synsets come from WordNet and contain the lemma in English
        if (bWordNet1 && bWordNet2)
        {
        	// both synsets are in the WordNet: sort based on the sense number
        	// of the senses of interest
        	int sNumber1 = 1000;
        	int sNumber2 = 1000;
            for (BabelSense s1 : b1.getSenses())
            {
                if (s1.getLemma().equalsIgnoreCase(lemma)
                	&& s1.getSource() == BabelSenseSource.WN)
                {
                	if(sNumber1 > s1.getSenseNumber())
                		sNumber1 = s1.getSenseNumber();
                }
            }
            
            for (BabelSense s2 : b2.getSenses())
            {
                if (s2.getLemma().equalsIgnoreCase(lemma)
                	&& s2.getSource() == BabelSenseSource.WN)
                {
                	if(sNumber2 > s2.getSenseNumber())
                		sNumber2 = s2.getSenseNumber();
                }
            }
            // do the magic ;)
            	return sNumber1 - sNumber2;
        }
        // if is WordNet and the senseKey is like "word"
        else if (bWordNet1) return -1;
        else if (bWordNet2) return 1;
        
        // synsets without WordNet-lemma matching 
        BabelSenseComparator bsc = new BabelSenseComparator(lemma);
        
        // compare the remaining types of senses
        List<BabelSense> b1senses = b1.getSenses(language, lemma, BabelSenseSource.OMWN, BabelSenseSource.WIKI, BabelSenseSource.WIKIRED, BabelSenseSource.OMWIKI);
        Collections.sort(b1senses, bsc);

        List<BabelSense> b2senses = b2.getSenses(language, lemma, BabelSenseSource.OMWN, BabelSenseSource.WIKI, BabelSenseSource.WIKIRED, BabelSenseSource.OMWIKI);
        Collections.sort(b2senses, bsc);
        
        if (b1senses.isEmpty()) return 1;
        if (b2senses.isEmpty()) return -1;
        return bsc.compare(b1senses.get(0), b2senses.get(0));
    }
}
