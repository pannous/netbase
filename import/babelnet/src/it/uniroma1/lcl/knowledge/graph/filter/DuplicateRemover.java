package it.uniroma1.lcl.knowledge.graph.filter;

import it.uniroma1.lcl.knowledge.KnowledgeBase;
import it.uniroma1.lcl.knowledge.graph.KnowledgeGraphPath;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

/**
 * Filters out all duplicate paths (that is, those with the same node sequence,
 * and possibly different relations).
 * 
 * @author ponzetto
 * 
 */
public class DuplicateRemover implements KnowledgeGraphPathFilter
{
	private static DuplicateRemover instance;

	private DuplicateRemover() { }

	public static synchronized DuplicateRemover getInstance()
	{
		if (null == instance) instance = new DuplicateRemover();
		return instance;
	}
	
	@Override
	public void filter(Collection<KnowledgeGraphPath> paths, KnowledgeBase kb)
	{
		Set<String> done = new HashSet<String>();
		Set<KnowledgeGraphPath> goodPaths = new HashSet<KnowledgeGraphPath>(); 
		
		for (KnowledgeGraphPath path : paths)
		{
			String conceptSequence = path.getConcepts().toString();

			if (done.contains(conceptSequence)) continue;
			else
			{
				done.add(conceptSequence);
				goodPaths.add(path);
			}
		}
		
		paths.retainAll(goodPaths);
	}
}
