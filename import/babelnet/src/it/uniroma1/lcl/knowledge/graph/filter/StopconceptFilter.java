package it.uniroma1.lcl.knowledge.graph.filter;

import it.uniroma1.lcl.knowledge.KnowledgeBase;
import it.uniroma1.lcl.knowledge.graph.KnowledgeGraphPath;

import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 * Filters out paths which contain stopword-like concepts.
 * 
 * @author ponzetto
 * 
 */
public class StopconceptFilter implements KnowledgeGraphPathFilter
{
	private static StopconceptFilter instance;

	private Set<String> stopconcepts;
	
	private StopconceptFilter()
	{
		this.stopconcepts = new HashSet<String>();
	}

	public static synchronized StopconceptFilter getInstance()
	{
		if (null == instance) instance = new StopconceptFilter();
		return instance;
	}
	
	public void filter(Collection<KnowledgeGraphPath> paths, KnowledgeBase kb)
	{
		Set<KnowledgeGraphPath> cleanPaths = new HashSet<KnowledgeGraphPath>();

		PathLoop: for (KnowledgeGraphPath path : paths)
		{
			// for each element in the path
			List<String> pathConcepts = path.getConcepts();
			for (int i = 0; i < pathConcepts.size(); i++)
			{
				String concept = pathConcepts.get(i);
				if (stopconcepts.contains(concept)) continue PathLoop;
			}
			cleanPaths.add(path);
		}

		paths.retainAll(cleanPaths);
	}
	
	/**
	 * Use for filter task-specific configuration
	 * 
	 * @param stopConcepts
	 */
	public void setStopconcepts(Set<String> stopConcepts)
	{
		this.stopconcepts = stopConcepts;
	}
}
