package it.uniroma1.lcl.knowledge.graph.filter;

import it.uniroma1.lcl.jlt.ling.Word;
import it.uniroma1.lcl.jlt.util.Language;
import it.uniroma1.lcl.jlt.util.Stopwords;
import it.uniroma1.lcl.knowledge.KnowledgeBase;
import it.uniroma1.lcl.knowledge.graph.KnowledgeGraphPath;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

/**
 * Filters out paths which contain stopwords, as found in a stopword list. NOTE:
 * works only for English at the moment.
 * 
 * @author ponzetto
 * 
 */
public class StopwordFilter implements KnowledgeGraphPathFilter
{
	private static StopwordFilter instance;

	private Set<String> stopwords;
	
	private StopwordFilter()
	{
		this.stopwords = Stopwords.getInstance().getStopwords();
	}

	public static synchronized StopwordFilter getInstance()
	{
		if (null == instance) instance = new StopwordFilter();
		return instance;
	}
	
	public void filter(Collection<KnowledgeGraphPath> paths, KnowledgeBase kb)
	{
		Set<String> doneConcepts = new HashSet<String>();
		Set<String> stopwordConcepts = new HashSet<String>();

		for (KnowledgeGraphPath path : paths)
		{
			// for each element in the path
			for (String concept : path.getConcepts())
			{
				if (!doneConcepts.contains(concept))
				{
					doneConcepts.add(concept);
					WordLoop: for (Word conceptWord : kb.getConceptWords(concept, Language.EN))
					{
						String word = conceptWord.getWord();
						for (String stopword : stopwords)
						{
							if (word.equalsIgnoreCase(stopword))
							{
								stopwordConcepts.add(concept);
								break WordLoop;
							}
						}
					}
				}
			}
		}

		Set<KnowledgeGraphPath> cleanPaths = new HashSet<KnowledgeGraphPath>();

		PathLoop: for (KnowledgeGraphPath path : paths)
		{
			// for each element in the path
			for (String concept : path.getConcepts())
			{
				if (stopwordConcepts.contains(concept)) continue PathLoop;
			}
			cleanPaths.add(path);
		}

		paths.retainAll(cleanPaths);
	}
	
	public void setStopwords(Set<String> stopwords)
	{
		this.stopwords = stopwords;
	}
}
