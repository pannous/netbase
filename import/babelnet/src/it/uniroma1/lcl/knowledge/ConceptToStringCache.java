package it.uniroma1.lcl.knowledge;

import it.uniroma1.lcl.babelnet.BabelNet;
import it.uniroma1.lcl.babelnet.iterators.BabelOffsetIterator;

import java.util.HashMap;
import java.util.Map;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * A cache of the BabelNet mappings concept id -> string
 * 
 * @author navigli
 *
 */
public class ConceptToStringCache
{
	private static final Log log = LogFactory.getLog(ConceptToStringCache.class);

	private static ConceptToStringCache instance;

	private final Map<String, String> conceptToString;
	
	private ConceptToStringCache()
	{
		this.conceptToString = new HashMap<String, String>();
		
		BabelNet bn = BabelNet.getInstance();
		BabelOffsetIterator i = bn.getOffsetIterator();
		
		int k = 0;
		while(i.hasNext())
		{
			String offset = i.next();
			conceptToString.put(offset, KnowledgeBase.BABELNET.conceptToString(offset));
			k++;
			if (k % 100000 == 1) log.info("Caching conceptId->string associations (1, ..., "+k+")");
		}
	}

	/**
	 * Provides access to <b>this</b> singleton {@link ConceptToStringCache}
	 * 
	 * @return
	 */
	public static synchronized ConceptToStringCache getInstance()
	{
		if (instance == null) instance = new ConceptToStringCache();
		return instance;
	}

	/**
	 * Gets the concept representation for an input conceptId.
	 * 
	 * @param conceptId
	 * @return
	 */
	public String get(String conceptId)
	{
		return conceptToString.get(conceptId);
	}

	/**
	 * Just for testing
	 * 
	 * @param args
	 */
	public static void main(String[] args)
	{
		try
		{
			ConceptToStringCache.getInstance();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
}
