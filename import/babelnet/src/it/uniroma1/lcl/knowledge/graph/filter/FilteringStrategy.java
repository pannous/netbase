package it.uniroma1.lcl.knowledge.graph.filter;

import it.uniroma1.lcl.knowledge.KnowledgeBase;
import it.uniroma1.lcl.knowledge.graph.KnowledgeGraphPath;

import java.util.Collection;

/**
 * An enumeration of different strategies to filter a {@link KnowledgeGraphPath}.
 * Basically used to wrap around different {@link KnowledgeGraphPathFilter},
 * e.g. for inclusion in configuration.
 * 
 * @author ponzetto
 * 
 */
public enum FilteringStrategy
{
	LOOP
	{
		public KnowledgeGraphPathFilter getFilter()
		{ return LoopFilter.getInstance(); }
	},
	
	EN_STOPWORDS
	{
		public KnowledgeGraphPathFilter getFilter()
		{ return StopwordFilter.getInstance(); }
	},
	
	STOPCONCEPTS
	{
		public KnowledgeGraphPathFilter getFilter()
		{ return StopconceptFilter.getInstance(); }
	},
	
	MIN_WEIGHT
	{
		public KnowledgeGraphPathFilter getFilter()
		{ return MinimumWeightFilter.getInstance(); }
	},
	
	REMOVE_DUPS
	{
		public KnowledgeGraphPathFilter getFilter()
		{ return DuplicateRemover.getInstance(); }
	},
	
	MAX_LENGTH
	{
		public KnowledgeGraphPathFilter getFilter()
		{ return MaxLengthFilter.getInstance(); }
	},
	
	ILLEGAL_POINTERS
	{
		public KnowledgeGraphPathFilter getFilter()
		{ return IllegalRelationFilter.getInstance(); }
	},
	
	LEGAL_POINTERS
	{
		public KnowledgeGraphPathFilter getFilter()
		{ return LegalRelationFilter.getInstance(); }
	},
	
	SENSE_SHIFTS
	{
		public KnowledgeGraphPathFilter getFilter()
		{ return SenseFilter.getInstance(); }
	};

	/**
	 * Delegates filtering to the underlying filter.
	 * 
	 * @param paths
	 * @param kb
	 */
	public void filter(Collection<KnowledgeGraphPath> paths, KnowledgeBase kb)
	{
		getFilter().filter(paths, kb);
	}
	
	public abstract KnowledgeGraphPathFilter getFilter();
}
