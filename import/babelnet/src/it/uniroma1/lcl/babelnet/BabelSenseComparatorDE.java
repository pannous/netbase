package it.uniroma1.lcl.babelnet;

import org.apache.commons.io.FileUtils;

import java.io.File;
import java.util.Comparator;
import java.util.HashMap;

/**
 * {@link java.util.Comparator} for {@link it.uniroma1.lcl.babelnet.BabelSense}s that (a) puts WordNet senses
 * first; (b) sorts WordNet senses based on their sense number; (c) sorts
 * Wikipedia senses lexicographically.
 *
 * @author navigli, ponzetto, vannella
 *
 */
public class BabelSenseComparatorDE implements Comparator<BabelSense>
{
    private static final HashMap<String, Integer> frequencies;

    static {
        frequencies = new HashMap<String, Integer>();
//        new File("").

    }

    private final String word;

	private int sortByLemma(BabelSense b1, BabelSense b2)
	{
	  String lemmab1 = b1.getSimpleLemma();
	  String lemmab2 = b2.getSimpleLemma();

	  // precedence to the specified word
	  if (word != null)
	  {
		  if (lemmab1.equalsIgnoreCase(word) && !lemmab2.equalsIgnoreCase(word)) return -1;
	  	  if (!lemmab1.equalsIgnoreCase(word) && lemmab2.equalsIgnoreCase(word)) return  1;

	  	  if (b1.getSynset().getSynsetType() == BabelSynsetType.CONCEPT && b2.getSynset().getSynsetType() == BabelSynsetType.NAMED_ENTITY)
	  		  return -1;
	  	  if (b2.getSynset().getSynsetType() == BabelSynsetType.CONCEPT && b1.getSynset().getSynsetType() == BabelSynsetType.NAMED_ENTITY)
	  		  return 1;
	  }

  	  return sortByParentheses(b1, b2);
	}

	private int sortByParentheses(BabelSense b1, BabelSense b2)
	{
		boolean bWiki1 = b1.getSource() == BabelSenseSource.WIKI;
		boolean bWiki2 = b2.getSource() == BabelSenseSource.WIKI;

		// precedence to WIKI vs. other sources
	  	if (bWiki1 && !bWiki2) return -1;
	    if (!bWiki1 && bWiki2) return 1;

	    boolean bPar1 = b1.getSenseString().contains("(");
	    boolean bPar2 = b2.getSenseString().contains("(");
		if (bPar1 && bPar2)
			return b1.getSenseString().toLowerCase().compareTo(b2.getSenseString().toLowerCase());
		else if (!bPar1) return -1;
		else if (!bPar2) return 1;
		// Wikipedia senses are sorted lexicographically otherwise
	    else return b1.getSenseString().toLowerCase().compareTo(b2.getSenseString().toLowerCase());
	}

	/**
	 * Creates a new instance of a {@link it.uniroma1.lcl.babelnet.BabelSenseComparatorDE}
	 *
	 * @param word
	 *            the word whose sense numbers are used to sort the
	 *            {@link it.uniroma1.lcl.babelnet.BabelSense}
	 * @author vannella
	 */
	public BabelSenseComparatorDE(String word)
	{
		this.word = word;
	}

	public BabelSenseComparatorDE()
	{
		this.word = null;
	}

//	@Override
    public int compare(BabelSense b1, BabelSense b2)
    {
        boolean bWordNet1 =
        	b1.getSource() == BabelSenseSource.WN;
        boolean bWordNet2 =
        	b2.getSource() == BabelSenseSource.WN;

        if (bWordNet1 && bWordNet2)
        {
        	// both senses are in the WordNet: sort based on the sense number
        	// of the senses of interest
            // do the magic ;)
        	String offset1 = b1.getWordNetOffset();
        	if (offset1 != null)
        	{
	        	int offsetResult = offset1.compareTo(b2.getWordNetOffset());

             	// if different WordNet synset
	        	if (offsetResult != 0) return b1.getSenseNumber()-(b2.getSenseNumber());
        	}
            else {
                Integer frequency1 = frequencies.get(b1);
                Integer frequency2 = frequencies.get(b2);
                return frequency2 - frequency1;
            }
            // if same WordNet synset
        	return b1.getPosition() - b2.getPosition();
        }
        // WordNet's senses come first than Wikipedia's
        else if (bWordNet1) return -1;
        // ditto
        else if (bWordNet2) return 1;

        // sort by source
        boolean bOMWordNet1 =
            	b1.getSource() == BabelSenseSource.OMWN;
            boolean bOMWordNet2 =
            	b2.getSource() == BabelSenseSource.OMWN;

		// sort open multiwordnet
		if (bOMWordNet1 && bOMWordNet2)
			return  b1.getSenseString().toLowerCase().compareTo(b2.getSenseString().toLowerCase());
		else if (bOMWordNet1) return -1;
		else if (bOMWordNet2) return 1;

		// sort wiki e wikired
		boolean bWiki1 =
			b1.getSource() == BabelSenseSource.WIKIRED || b1.getSource() == BabelSenseSource.WIKI;
		boolean bWiki2 =
			b2.getSource() == BabelSenseSource.WIKIRED || b2.getSource() == BabelSenseSource.WIKI;

		// sort by "("
		if (bWiki1 && bWiki2) return sortByLemma(b1, b2);
		else if (bWiki1) return -1;
		else if (bWiki2) return 1;

		else return b1.getSenseString().toLowerCase().compareTo(b2.getSenseString().toLowerCase());
	}
}
