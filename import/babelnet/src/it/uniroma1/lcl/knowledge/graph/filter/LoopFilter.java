package it.uniroma1.lcl.knowledge.graph.filter;

import it.uniroma1.lcl.knowledge.KnowledgeBase;
import it.uniroma1.lcl.knowledge.graph.KnowledgeGraphPath;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

/**
 * Filters out paths containing loops.
 * 
 * @author ponzetto
 * 
 */
public class LoopFilter implements KnowledgeGraphPathFilter
{
	private static LoopFilter instance;

	private LoopFilter() { }

	public static synchronized LoopFilter getInstance()
	{
		if (null == instance) instance = new LoopFilter();
		return instance;
	}
	
	@Override
	public void filter(Collection<KnowledgeGraphPath> paths, KnowledgeBase kb)
	{
		List<KnowledgeGraphPath> remove = new ArrayList<KnowledgeGraphPath>();
		for (KnowledgeGraphPath path : paths)
			if (path.hasLoop()) remove.add(path);
		paths.removeAll(remove);
	}
}
