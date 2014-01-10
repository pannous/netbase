package it.uniroma1.lcl.knowledge.graph;

import it.uniroma1.lcl.jlt.jgrapht.WeightedLabeledEdge;
import it.uniroma1.lcl.jlt.ling.Word;
import it.uniroma1.lcl.knowledge.KnowledgeBase;
import it.uniroma1.lcl.knowledge.KnowledgeBasePathIndex;
import it.uniroma1.lcl.knowledge.KnowledgeConfiguration;
import it.uniroma1.lcl.knowledge.graph.filter.KnowledgeGraphPathFilter;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.jgrapht.DirectedGraph;

import com.google.common.collect.HashMultimap;
import com.google.common.collect.Multimap;

/**
 * A class to create {@link KnowledgeGraph}s from a {@link KnowledgeBasePathIndex}.
 * 
 * @author ponzetto
 * 
 */
public class KnowledgeGraphFactory
{
	protected static final Log log = LogFactory.getLog(KnowledgeGraphFactory.class);

	private static Map<KnowledgeBase, KnowledgeGraphFactory> singleton = 
		new HashMap<KnowledgeBase, KnowledgeGraphFactory>();
	
	public static synchronized KnowledgeGraphFactory getInstance(KnowledgeBase kb)
	{
		if (!singleton.containsKey(kb))
			singleton.put(kb, new KnowledgeGraphFactory(kb));
		return singleton.get(kb);
	}
	
	private boolean verbose;
	
	protected final KnowledgeBase kb;
	
	protected final KnowledgeBasePathIndex index;
	
	protected KnowledgeGraphFactory(KnowledgeBase kb)
	{
		this.kb = kb;
		this.index = new KnowledgeBasePathIndex(kb);
	}
	
	/**
	 * Creates a {@link KnowledgeGraph} by exploring the graph of the knowledge
	 * base starting from and ending to the given words.
	 * 
	 * @param words
	 * @return a {@link KnowledgeGraph} for the input words
	 * @throws IOException
	 */
	public KnowledgeGraph getKnowledgeGraph(Collection<Word> words) throws IOException
	{
		return getKnowledgeGraph(words, words, null);
	}
	
	/**
	 * Creates a {@link KnowledgeGraph} by exploring the graph of the knowledge
	 * base from the given source words and ending in any of the context words.
	 * 
	 * @param sourceWords
	 * @param contextWords
	 *            if <b>null</b> explores the graph at maximum depth with no
	 *            end-point (ie stopping at context words)
	 * @return a {@link KnowledgeGraph} for the input words
	 * @throws IOException
	 */
	public KnowledgeGraph getKnowledgeGraph(Collection<Word> sourceWords,
			 								Collection<Word> contextWords) throws IOException
	{
		return getKnowledgeGraph(sourceWords, contextWords, null);
	}
	
	/**
	 * Creates a {@link KnowledgeGraph} by exploring the graph of the knowledge
	 * base from the given source words and ending in any of the context words,
	 * additionally filtering the paths with a bunch of
	 * {@link KnowledgeGraphPathFilter}
	 * 
	 * @param sourceWords
	 * @param contextWords
	 *            if <b>null</b> explores the graph at maximum depth with no
	 *            end-point
	 * @param filters
	 * @return a {@link KnowledgeGraph} for the input words
	 * @throws IOException
	 */
	public KnowledgeGraph getKnowledgeGraph(Collection<Word> sourceWords,
											Collection<Word> contextWords,
											Collection<KnowledgeGraphPathFilter> filters) throws IOException
	{
		log.info(
				"CREATING DISAMBIGUATION GRAPH\n\t\t\t PATHS START FROM: " + sourceWords +
				"\n\t\t\t CONTEXT:" + contextWords);
		
		Set<String> sourceConcepts = new HashSet<String>();
		Set<String> contextConcepts = new HashSet<String>();
		
		Multimap<Word, String> sourceWordToConcept = new HashMultimap<Word, String>();

		for (Word word : sourceWords)
		{
			for (String concept : kb.getConcepts(word))
			{
				sourceConcepts.add(concept);
				sourceWordToConcept.put(word, concept);
			}
		}
		
		for (Word word : contextWords)
			for (String concept : kb.getConcepts(word))
				contextConcepts.add(concept);
		
		return getKnowledgeGraph(sourceConcepts, sourceWordToConcept,
								contextWords, contextConcepts, filters);
	}

	/**
	 * Creates a {@link KnowledgeGraph} by exploring the graph of the knowledge
	 * base starting from and ending to the given concept ids.
	 * 
	 * @param concepts
	 * @return a {@link KnowledgeGraph} for the input concept ids
	 * @throws IOException
	 */
	public KnowledgeGraph getKnowledgeGraphForConcepts(Collection<String> concepts) throws IOException
	{
		return getKnowledgeGraphForConcepts(concepts, concepts, null);
	}
	
	/**
	 * Creates a {@link KnowledgeGraph} by exploring the graph of the knowledge
	 * base from the given source words and ending in any of the concepts.
	 * 
	 * @param sourceConcepts
	 * @param contextConcepts
	 *            if <b>null</b> explores the graph at maximum depth with no
	 *            end-point
	 * @return a {@link KnowledgeGraph} for the input concept ids
	 * @throws IOException
	 */
	public KnowledgeGraph getKnowledgeGraphForConcepts(Collection<String> sourceConcepts,
	 												   Collection<String> contextConcepts) throws IOException
	{
		return getKnowledgeGraphForConcepts(sourceConcepts, contextConcepts, null);
	}
	
	/**
	 * Creates a {@link KnowledgeGraph} by exploring the graph of the knowledge
	 * base from the given source concepts and ending in any of the specified
	 * concepts, additionally filtering the paths with a bunch of
	 * {@link KnowledgeGraphPathFilter}
	 * 
	 * @param sourceWords
	 * @param contextWords
	 *            if <b>null</b> explores the graph at maximum depth with no
	 *            end-point
	 * @param filters
	 * @return a {@link KnowledgeGraph} for the input words
	 * @throws IOException
	 */
	public KnowledgeGraph getKnowledgeGraphForConcepts(Collection<String> sourceConcepts,
			 								 		   Collection<String> contextConcepts,
			 								 		   Collection<KnowledgeGraphPathFilter> filters) throws IOException
	{
		log.info(
				"CREATING DISAMBIGUATION GRAPH\n\t\t\t PATHS START FROM: " + sourceConcepts +
				"\n\t\t\t CONTEXT:" + contextConcepts);
		
		return getKnowledgeGraph(sourceConcepts, null, null, contextConcepts, filters);
	}
	
	/**
	 * The actual factory method
	 * 
	 * @param sourceConcepts
	 * @param sourceWord2concepts
	 * @param contextWords
	 * @param contextConcepts
	 * @param filters
	 * @return
	 * @throws IOException
	 */
	private KnowledgeGraph getKnowledgeGraph(Collection<String> sourceConcepts,
											 Multimap<Word, String> sourceWord2concepts,
											 Collection<Word> contextWords,
											 Collection<String> contextConcepts,
											 Collection<KnowledgeGraphPathFilter> filters) throws IOException
	{
		// STEP 1: takes the paths
		Set<KnowledgeGraphPath> paths =
			getPaths(sourceConcepts, contextConcepts, filters);
		
		// creates the hash from source to their paths
		Multimap<String, KnowledgeGraphPath> concept2paths = 
			new HashMultimap<String, KnowledgeGraphPath>();
		// creates a map containing paths to each vertex
		Multimap<String, KnowledgeGraphPath> paths2concept =
			new HashMultimap<String, KnowledgeGraphPath>();
		
		for (KnowledgeGraphPath path : paths)
		{
			List<String> pathConcepts = path.getConcepts();
			concept2paths.put(pathConcepts.get(0), path);
			paths2concept.put(pathConcepts.get(pathConcepts.size()-1), path);
		}
		
		// STEP 2: builds the graph
		DirectedGraph<String, WeightedLabeledEdge> graph = 
			KnowledgeGraphPath.toUnionGraph(concept2paths.values());

		// STEP 3: creates the map from words to nodes in the graph
		Multimap<Word, String> pathSourceWord2concepts = new HashMultimap<Word, String>();
		if (sourceWord2concepts != null)
			for (Word word : sourceWord2concepts.keySet())
				for (String concept : sourceWord2concepts.get(word))
					if (concept2paths.containsKey(concept))
						pathSourceWord2concepts.put(word, concept);
			
		// LAST STEP: creates the actual graph
		return new KnowledgeGraph(graph, concept2paths, paths2concept, 
								   pathSourceWord2concepts, contextWords, kb);
	}
	
	private Set<KnowledgeGraphPath> getPaths(Collection<String> sourceConcepts,
											 Collection<String> contextConcepts,
						  					 Collection<KnowledgeGraphPathFilter> filters) throws IOException
	{	
		// map from offset to paths originating with that offset
		Multimap<String, List<String>> firstConcept2pathsStartingWithSourceSense = 
			new HashMultimap<String, List<String>>();
		// map from offset to paths ending with that offset
		Multimap<String, List<String>> firstConcept2pathsEndingWithContextSense = 
			new HashMultimap<String, List<String>>();
		
		// collects the single paths
		for (String concept : sourceConcepts)
		{
			Set<List<String>> pathsFrom = index.getPathsFrom(concept);
			for (List<String> pathFrom : pathsFrom)
			{
				firstConcept2pathsStartingWithSourceSense.put(pathFrom.get(0),
															  pathFrom);
			}
		}
		
		for (String concept : contextConcepts)
		{
			Set<List<String>> pathsTo = index.getPathsTo(concept, 1);
			for (List<String> pathTo : pathsTo)
			{
				firstConcept2pathsEndingWithContextSense.put(pathTo.get(0),
															 pathTo);
			}
		}
		
		// the set of paths between senses of the input words
		Set<KnowledgeGraphPath> paths = new HashSet<KnowledgeGraphPath>();
		
		// for each concept
		for (String concept : sourceConcepts)
		{
			// the paths starting with that concept
			Collection<List<String>> pathsFromConcept =
				firstConcept2pathsStartingWithSourceSense.get(concept);
			
			for (List<String> pathFromConcept : pathsFromConcept)
			{
				// the last element of the path
				String rightElement = pathFromConcept.get(pathFromConcept.size()-1);

				if (
					// no context: we take everything
						contextConcepts == null
					||
						contextConcepts.isEmpty()
					||
					// we landed directly on a context word
					contextConcepts.contains(rightElement))
				{
					// save the path
					KnowledgeGraphPath path =
						new KnowledgeGraphPath(pathFromConcept, kb);
					paths.add(path);
				}
				else
				{
					// get the paths from the last element landing in a
					// context word
					Collection<List<String>> pathsFromRightElement =
						firstConcept2pathsEndingWithContextSense.get(rightElement);
					for (List<String> pathFromRightElement : pathsFromRightElement)
					{
						String rightElement2 = 
							pathFromRightElement.get(pathFromRightElement.size()-1);
						if (contextConcepts.contains(rightElement2))
						{
							// attaches the two paths
							List<String> pathList = new ArrayList<String>();
							
							// "left" side
							for (int i = 0; i < pathFromConcept.size()-1; i++)
								pathList.add(pathFromConcept.get(i));
							
							// "right" side
							for (int i = 0; i < pathFromRightElement.size(); i++)
								pathList.add(pathFromRightElement.get(i));

							KnowledgeGraphPath path = 
								new KnowledgeGraphPath(pathList, kb);
							paths.add(path);
						}
						else
							throw new RuntimeException("Invalid path: " + pathFromRightElement);
					}
				}
			}
		}
		
		// filter if appropriate
		Collection<KnowledgeGraphPathFilter> pathFilters;
		if (filters == null)
			pathFilters = KnowledgeConfiguration.getInstance().getKnowledgeGraphPathFilters();
		else
			pathFilters = filters;
		
		if (verbose) log.info("FOUND " + paths.size() + " PATHS (before filtering)");
		for (KnowledgeGraphPathFilter pathFilter : pathFilters)
		{
			if (verbose) log.info("FILTERING PATHS WITH " + pathFilter.getClass().getSimpleName());
			pathFilter.filter(paths, kb);
		}
		if (verbose) log.info("FOUND " + paths.size() + " PATHS (after filtering)");
		
		return paths;
	}
	
	public void setVerbose(boolean verbose)
	{
		this.verbose = verbose;
	}
	
	public void close() throws IOException
	{
		index.close();
	}
	
	public static void closeInstances() throws IOException
	{
		for (KnowledgeBase kb : singleton.keySet())
		{
			KnowledgeGraphFactory disGraphFactory = singleton.get(kb);
			disGraphFactory.close();
		}
	}
}
