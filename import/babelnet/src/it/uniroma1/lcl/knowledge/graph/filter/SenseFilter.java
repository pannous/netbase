package it.uniroma1.lcl.knowledge.graph.filter;

import it.uniroma1.lcl.jlt.ling.Word;
import it.uniroma1.lcl.jlt.util.Language;
import it.uniroma1.lcl.knowledge.KnowledgeBase;
import it.uniroma1.lcl.knowledge.graph.KnowledgeGraphPath;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

import com.google.common.collect.HashMultimap;
import com.google.common.collect.Multimap;

/**
 * Filters out paths which have sense shifts, that is, they contain different
 * senses of the same word -- as lexicalized in a certain language within a
 * {@link KnowledgeBase}.
 * 
 * @author ponzetto
 * 
 */
public class SenseFilter implements KnowledgeGraphPathFilter
{
	private static SenseFilter instance;

	private Language language;
	
	private SenseFilter()
	{
		this.language = Language.EN;
	}

	public static synchronized SenseFilter getInstance()
	{
		if (null == instance) instance = new SenseFilter();
		return instance;
	}
	
	@Override
	public void filter(Collection<KnowledgeGraphPath> paths, KnowledgeBase kb)
	{
		Multimap<String, Word> concept2words = new HashMultimap<String, Word>();
		for (KnowledgeGraphPath path : paths)
		{
			// for each element in the path
			for (String concept : path.getConcepts())
			{
				if (!concept2words.containsKey(concept))
				{
					for (Word conceptWord : kb.getConceptWords(concept, language))
						concept2words.put(concept, new Word(conceptWord.getWord().toLowerCase(),
															conceptWord.getPOS().getTag()));
				}
			}
		}
		
		Set<KnowledgeGraphPath> cleanPaths = new HashSet<KnowledgeGraphPath>(); 
		
		PathLoop:
		for (KnowledgeGraphPath path : paths)
		{
			// hash of words seen
			Set<Word> wordsDone = new HashSet<Word>();
		
			// for each element in the path
			for (String concept : path.getConcepts())
			{
				for (Word lexicalization : concept2words.get(concept))
				{
					// have we seen this word already?
					if (wordsDone.contains(lexicalization)) continue PathLoop;
					wordsDone.add(lexicalization);
				}
			}
			cleanPaths.add(path);
		}
		
		paths.retainAll(cleanPaths);
	}
	
	/**
	 * Use for filter task-specific configuration
	 * 
	 * @param language
	 */
	public void setLanguage(Language language)
	{
		this.language = language;
	}
}
