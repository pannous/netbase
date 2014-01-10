package it.uniroma1.lcl.knowledge.graph;

import it.uniroma1.lcl.jlt.jgrapht.WeightedLabeledEdge;
import it.uniroma1.lcl.jlt.jung.VerbosePageRank;
import it.uniroma1.lcl.jlt.util.Maths;

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import org.jgrapht.DirectedGraph;

import com.google.common.collect.Multimap;

import edu.uci.ics.jung.algorithms.scoring.PageRank;
import edu.uci.ics.jung.graph.DirectedSparseGraph;
import edu.uci.ics.jung.graph.Graph;

/**
 * An enumeration of different strategies to score a {@link KnowledgeGraph}.
 * 
 * @author ponzetto
 * 
 */
public enum KnowledgeGraphScorer
{
	/**
	 * Assign to each node of interest it degree
	 */
	DEGREE
	{
		@Override
		public Map<String, Double> score(KnowledgeGraph disambiguationGraph,
										 Collection<String> verticesToScore)
		{
			DirectedGraph<String, WeightedLabeledEdge> graph = 
				disambiguationGraph.getGraph();
			Map<String, Double> scores = new HashMap<String, Double>();
			
			for (String vertex : verticesToScore)
				if (graph.containsVertex(vertex))
					scores.put(vertex,(double)graph.outDegreeOf(vertex));

			return scores;
		}
	},

	/**
	 * Assign to each node of interest the sum of the inverse length of the
	 * paths starting from it
	 * 
	 * @see KnowledgeGraphPathScorer#INVERSE_PATH_LENGTH
	 * 
	 */
	SUM_INVERSE_PATH_LENGTH
	{
		@Override
		public Map<String, Double> score(KnowledgeGraph disambiguationGraph,
										  Collection<String> verticesToScore)
		{
			return score(disambiguationGraph,
						  verticesToScore,
						  KnowledgeGraphPathScorer.INVERSE_PATH_LENGTH);
		}
	},
	
	/**
	 * Assign to each node of interest the sum of the weights of the
	 * paths starting from it
	 * 
	 * @see KnowledgeGraphPathScorer#PATH_PROBABILITY
	 * 
	 */
	SUM_PATH_PROBABILITY
	{
		@Override
		public Map<String, Double> score(KnowledgeGraph disambiguationGraph,
										  Collection<String> verticesToScore)
		{
			return score(disambiguationGraph,
						  verticesToScore,
						  KnowledgeGraphPathScorer.PATH_PROBABILITY);
		}
	},
	
	/**
	 * Assign to each node of interest it pagerank score
	 */
	PAGERANK
	{
		@Override
		public Map<String, Double> score(KnowledgeGraph disambiguationGraph,
										  Collection<String> verticesToScore)
        {
			DirectedGraph<String, WeightedLabeledEdge> jgraph = 
				disambiguationGraph.getGraph();
			
			Graph<String, String> jungGraph = new DirectedSparseGraph<String, String>();
			for (String vertex : jgraph.vertexSet()) jungGraph.addVertex(vertex);
			for (WeightedLabeledEdge e : jgraph.edgeSet())
			{
				String source = jgraph.getEdgeSource(e);
				String target = jgraph.getEdgeTarget(e);
				String edgeLabel = source+":"+target;
				jungGraph.addEdge(edgeLabel, source, target);
			}
			
			PageRank<String, String> pr =
				new VerbosePageRank<String, String>(jungGraph);
			pr.evaluate();
			
			Map<String, Double> scores = new HashMap<String, Double>();
			for (String vertex : verticesToScore)
			{
				if (jungGraph.containsVertex(vertex))
				{
					double score = 0.0;
					score = pr.getVertexScore(vertex);
					scores.put(vertex, score);
				}
			}
			return scores;
		}
	},
	
	/**
	 * A simple placeholder to signal that we are scoring with the upper bound
	 * 
	 */
	UPPER_BOUND
	{
		@Override
		public Map<String, Double> score(KnowledgeGraph disambiguationGraph,
										  Collection<String> verticesToScore)
		{
			return null;
		}
	};
	
	/**
	 * Assign to each node of interest the sum of the score of the paths
	 * starting from it
	 * 
	 * @param disambiguationGraph
	 * @param verticesToScore
	 * @param scorer
	 * @return a {@link Map} from nodes to scores for all input vertices found
	 *         within the given graph
	 */
	public static Map<String, Double> score(KnowledgeGraph disambiguationGraph,
			  						  		Collection<String> verticesToScore,
			  						  		KnowledgeGraphPathScorer scorer)
	{
		Multimap<String, KnowledgeGraphPath> concept2paths =
			disambiguationGraph.getConcept2paths();
		Map<String, Double> scores = new HashMap<String, Double>();

		for (String vertex : verticesToScore)
		{
			double score = 0.0;
			Collection<KnowledgeGraphPath> paths = concept2paths.get(vertex);
			for (KnowledgeGraphPath path : paths)
			{
				double pathScore = scorer.score(path);
				score += pathScore;
			}
			scores.put(vertex, Maths.round(score, 10));
		}

		return scores;
	}
	
	/**
	 * Returns a scoring for <b>all</b> input vertices found in the
	 * {@link KnowledgeGraph}.
	 * 
	 * @param disambiguationGraph
	 * @return a {@link Map} from nodes to scores for all input vertices found
	 *         within the given graph
	 */
	public Map<String, Double> score(KnowledgeGraph disambiguationGraph)
	{
		return score(disambiguationGraph, disambiguationGraph.getGraph().vertexSet());
	}
	
	/**
	 * Returns a scoring for <b>specific</b> vertices found in the
	 * {@link KnowledgeGraph}.
	 * 
	 * @param disambiguationGraph
	 * @param verticesToScore
	 *            the collections of vertices to score
	 * @return a {@link Map} from nodes to scores for the specified vertices
	 *         found within the given graph
	 */
	public abstract Map<String, Double> score(KnowledgeGraph disambiguationGraph,
											  Collection<String> verticesToScore);
}
