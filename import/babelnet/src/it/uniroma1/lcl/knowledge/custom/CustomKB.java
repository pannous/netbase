package it.uniroma1.lcl.knowledge.custom;

import it.uniroma1.lcl.jlt.ling.Word;
import it.uniroma1.lcl.jlt.util.Language;
import it.uniroma1.lcl.jlt.util.ScoredItem;
import it.uniroma1.lcl.knowledge.KnowledgeBase;
import it.uniroma1.lcl.knowledge.KnowledgeConfiguration;

import java.util.Iterator;
import java.util.List;
import java.util.Set;

import com.google.common.collect.Multimap;

import edu.mit.jwi.item.IPointer;

/**
 * An abstract class to model a custom knowldge base to be plugged in
 * {@link KnowledgeBase} infrastructure.
 * 
 * @author flati
 *
 */
public abstract class CustomKB
{
	private static CustomKB kb;

	public static synchronized CustomKB getInstance()
	{
		if (kb == null) kb = KnowledgeConfiguration.getInstance().getKnowledgeBase();
		return kb;
	}
	
	public abstract Multimap<Language, Word> getConceptWordsByLanguage(String concept, Language language);

	public abstract List<String> getConcepts(Word word);

	public abstract Set<String> getRelatedConcepts(String concept, Language language);

	public abstract Multimap<IPointer, ScoredItem<String>> getRelatedConceptsMap(String concept, Language language);

	public abstract String conceptToString(String concept, Language language,boolean verbose);

	public abstract IPointer getPointer(String pointerSymbol);

	public abstract Iterator<String> getConceptIterator();
}