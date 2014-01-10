package it.uniroma1.lcl.knowledge;


import java.util.Iterator;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import edu.uci.ics.jung.graph.DirectedSparseGraph;
import edu.uci.ics.jung.graph.Graph;
import edu.uci.ics.jung.graph.UndirectedSparseGraph;

/**
 * Utility methods to work with {@link KnowledgeBase}s.
 * 
 * @author ponzetto
 *
 */
public class KnowledgeBases
{
	private static final Log log = LogFactory.getLog(KnowledgeBases.class);
	
	/**
	 * Converts a {@link KnowledgeBase} to an undirected graph
	 * 
	 * @param kb
	 * @return an {@link UndirectedSparseGraph} from the input KB
	 */
	public static Graph<String, String> toJungUndirectedGraph(KnowledgeBase kb)
	{
		return toJungGraph(kb, new UndirectedSparseGraph<String, String>());
	}

	/**
	 * Converts a {@link KnowledgeBase} to a directed graph
	 * 
	 * @param kb
	 * @return a {@link DirectedSparseGraph} from the input KB
	 */
	public static Graph<String, String> toJungDirectedGraph(KnowledgeBase kb)
	{
		return toJungGraph(kb, new DirectedSparseGraph<String, String>());
	}
	
	private static Graph<String, String> toJungGraph(KnowledgeBase kb,
													 Graph<String, String> graph)
	{
		log.info("CREATING A JUNG GRAPH FROM " + kb);
		Iterator<String> conceptIterator = kb.getConceptIterator();			

		int counter = 0;
		while (conceptIterator.hasNext())
		{
			String concept = conceptIterator.next();
			graph.addVertex(concept);
			
			for (String related : kb.getRelatedConcepts(concept))
			{
				String edgeLabel = concept + ":" + related;
				graph.addVertex(related);
				graph.addEdge(edgeLabel, concept, related);
			}
			if ((counter%10000)==0) log.info(" LOADED " + counter + " NODES SO FAR ...");
			counter++;
		}
		
		log.info("JUNG GRAPH CREATED");
		return graph;
	}
}
