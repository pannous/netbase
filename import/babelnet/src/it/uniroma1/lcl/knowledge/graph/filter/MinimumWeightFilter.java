package it.uniroma1.lcl.knowledge.graph.filter;

import it.uniroma1.lcl.knowledge.KnowledgeBase;
import it.uniroma1.lcl.knowledge.KnowledgeConfiguration;
import it.uniroma1.lcl.knowledge.graph.KnowledgeGraphPath;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

/**
 * Filters out all paths that contain <b>at least one</b> edge with weight
 * below a certain threshold.
 *
 * @author ponzetto
 *
 */
public class MinimumWeightFilter implements KnowledgeGraphPathFilter
{
	private static MinimumWeightFilter instance;

	private double minWeight;
	
	private MinimumWeightFilter()
	{
		this.minWeight =
			KnowledgeConfiguration.getInstance().
				getConceptGraphPathFilterWeigthThreshold();
	}

	public static synchronized MinimumWeightFilter getInstance()
	{
		if (null == instance) instance = new MinimumWeightFilter();
		return instance;
	}
	
	@Override
	public void filter(Collection<KnowledgeGraphPath> paths, KnowledgeBase kb)
	{
		Set<KnowledgeGraphPath> goodPaths = new HashSet<KnowledgeGraphPath>(); 
		
		PathLoop:
		for (KnowledgeGraphPath path : paths)
		{
			for (Double weight : path.getWeights())
			{
				if (weight < minWeight) continue PathLoop;
			}
			goodPaths.add(path);
		}
		
		paths.retainAll(goodPaths);
	}

	/**
	 * Use for filter task-specific configuration
	 * 
	 * @param minWeight
	 */
	public void setMinWeight(double minWeight)
	{
		this.minWeight = minWeight;
	}
}
