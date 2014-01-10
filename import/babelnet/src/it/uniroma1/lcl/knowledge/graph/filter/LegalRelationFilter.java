package it.uniroma1.lcl.knowledge.graph.filter;

import it.uniroma1.lcl.knowledge.KnowledgeBase;
import it.uniroma1.lcl.knowledge.KnowledgeConfiguration;
import it.uniroma1.lcl.knowledge.graph.KnowledgeGraphPath;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

/**
 * Filters out paths not containing any of the input relations.
 *
 * @author ponzetto
 *
 */
public class LegalRelationFilter implements KnowledgeGraphPathFilter
{
	private static LegalRelationFilter instance;

	private List<String> legalPointers;
	
	private LegalRelationFilter()
	{
		this.legalPointers =
			KnowledgeConfiguration.getInstance().getConceptGraphPathLegalPointers();
	}

	public static synchronized LegalRelationFilter getInstance()
	{
		if (null == instance) instance = new LegalRelationFilter();
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
				if (!legalPointers.contains(pointer)) 
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
	 * @param legalPointers
	 */
	public void setLegalPointers(List<String> legalPointers)
	{
		this.legalPointers = legalPointers;
	}
}
