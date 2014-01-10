package it.uniroma1.lcl.knowledge.graph.filter;

import java.util.Collection;

import it.uniroma1.lcl.knowledge.KnowledgeBase;
import it.uniroma1.lcl.knowledge.graph.KnowledgeGraphPath;

/**
 * The common interface for different strategies used to filter a
 * {@link KnowledgeGraphPath}.
 * 
 * @author ponzetto
 * 
 */
public interface KnowledgeGraphPathFilter
{
	/**
	 * Filters a collection of paths, possibly using a specific KB, e.g. to
	 * extract the concepts lexicalizations.
	 * 
	 * @param paths
	 * @param kb
	 */
	void filter(Collection<KnowledgeGraphPath> paths, KnowledgeBase kb);
}
