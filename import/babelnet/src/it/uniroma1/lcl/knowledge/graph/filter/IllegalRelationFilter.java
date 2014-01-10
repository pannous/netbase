package it.uniroma1.lcl.knowledge.graph.filter;

import it.uniroma1.lcl.knowledge.KnowledgeBase;
import it.uniroma1.lcl.knowledge.KnowledgeConfiguration;
import it.uniroma1.lcl.knowledge.graph.KnowledgeGraphPath;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

/**
 * Filters out paths containing any of the input relations.
 *
 * @author ponzetto
 *
 */
public class IllegalRelationFilter implements KnowledgeGraphPathFilter
{
	private static IllegalRelationFilter instance;

	private List<String> illegalPointers;
	
	private IllegalRelationFilter()
	{
		this.illegalPointers =
			KnowledgeConfiguration.getInstance().getConceptGraphPathIllegalPointers();
	}

	public static synchronized IllegalRelationFilter getInstance()
	{
		if (null == instance) instance = new IllegalRelationFilter();
		return instance;
	}
	
	@Override
	public void filter(Collection<KnowledgeGraphPath> paths, KnowledgeBase kb)
	{
		List<KnowledgeGraphPath> remove = new ArrayList<KnowledgeGraphPath>();

		PathLoop:
		for (KnowledgeGraphPath path : paths)
		{
			for (String pointer : path.getPointerSymbols())
			{
				if (illegalPointers.contains(pointer)) 
				{
					remove.add(path);
					continue PathLoop;
				}
			}
		}
		
		paths.removeAll(remove);
	}
	
	/**
	 * Use for filter task-specific configuration.
	 * 
	 * @param illegalPointers
	 */
	public void setIllegalPointers(List<String> illegalPointers)
	{
		this.illegalPointers = illegalPointers;
	}
}
