package it.uniroma1.lcl.knowledge.graph.filter;

import it.uniroma1.lcl.knowledge.KnowledgeBase;
import it.uniroma1.lcl.knowledge.KnowledgeConfiguration;
import it.uniroma1.lcl.knowledge.graph.KnowledgeGraphPath;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

/**
 * Filters out all paths with length below a certain threshold.
 * 
 * @author ponzetto
 * 
 */
public class MaxLengthFilter implements KnowledgeGraphPathFilter
{
	private static MaxLengthFilter instance;

	private int maxLength;
	
	private MaxLengthFilter()
	{
		this.maxLength = 
			KnowledgeConfiguration.getInstance().
				getConceptGraphPathFilterLengthThreshold();		
	}

	public static synchronized MaxLengthFilter getInstance()
	{
		if (null == instance) instance = new MaxLengthFilter();
		return instance;
	}

	@Override
	public void filter(Collection<KnowledgeGraphPath> paths, KnowledgeBase kb)
	{
		Set<KnowledgeGraphPath> goodPaths = new HashSet<KnowledgeGraphPath>(); 
		
		for (KnowledgeGraphPath path : paths)
		{
			if (path.getPointerSymbols().size() > maxLength) continue;
			else goodPaths.add(path);
		}
		
		paths.retainAll(goodPaths);
	}
	
	/**
	 * Use for filter task-specific configuration.
	 * 
	 * @param maxLength
	 */
	public void setMaxLength(int maxLength)
	{
		this.maxLength = maxLength;
	}
}
