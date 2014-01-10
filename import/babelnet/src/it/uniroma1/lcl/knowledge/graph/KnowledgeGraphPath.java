package it.uniroma1.lcl.knowledge.graph;

import it.uniroma1.lcl.jlt.jgrapht.WeightedLabeledEdge;
import it.uniroma1.lcl.knowledge.KnowledgeBase;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.jgrapht.DirectedGraph;
import org.jgrapht.Graph;
import org.jgrapht.Graphs;
import org.jgrapht.graph.DefaultDirectedGraph;
import org.jgrapht.graph.DefaultEdge;
import org.jgrapht.graph.DirectedMultigraph;

import com.google.common.collect.HashMultimap;
import com.google.common.collect.Multimap;
import com.google.common.collect.SetMultimap;
import com.google.common.collect.Sets;

import edu.mit.jwi.item.IPointer;

/**
 * A path in a {@link KnowledgeGraph}.
 * 
 * @author ponzetto
 * 
 */
public class KnowledgeGraphPath
{
	private final List<String> path;
	
	private final List<String> concepts;
	
	private final List<IPointer> pointers;
	
	private final List<String> pointerSymbols;
	
	private final List<Double> weights;

	private final List<KnowledgeGraphPathEdge> edges;
	
	public KnowledgeGraphPath(List<String> path, KnowledgeBase kb)
	{
		this.path = path;
		
		this.concepts = new ArrayList<String>();
		this.pointers = new ArrayList<IPointer>();
		this.pointerSymbols = new ArrayList<String>();
		this.edges =  new ArrayList<KnowledgeGraphPathEdge>();
		this.weights = new ArrayList<Double>();
		
		for (int i = 0; i < path.size(); i++)
		{
			int position = i % 3;
			String element = path.get(i);
			switch (position)
			{
				case 0:
					// offsets
					concepts.add(element);
					break;
				case 1:
					// pointer (symbols)
					IPointer pointer = kb.getPointer(element);
					pointers.add(pointer);
					String pointerSymbol = element; 
					pointerSymbols.add(pointerSymbol);
					// edges
					String src = path.get(i-1);
					Double weight = Double.parseDouble(path.get(i+1));
					String target = path.get(i+2);
					KnowledgeGraphPathEdge edge =
						new KnowledgeGraphPathEdge(src, target, 
														pointerSymbol, pointer,
														weight);
					edges.add(edge);
					break;
				case 2:
					// weights
					weights.add(Double.parseDouble(element));
					break;
				default:
					break;
			}
		}
	}
	
	public List<String> getPath() 	{ return path; }
	
	/**
	 * Returns all the concepts in the path
	 * @return all the concepts in the path
	 */
	public List<String> getConcepts() { return concepts; }
	
	/**
	 * Returns the first <i>n</i> concepts in the path
	 * 
	 * @param n the number of concepts to return
	 * @return the first <i>n</i> concepts in the path
	 */
	public List<String> getConcepts(int n)
	{
		if (n < concepts.size())
			return concepts.subList(0, n);
		else
			return concepts;
	}

	public List<IPointer> getPointers() { return pointers; }
	
	public List<String> getPointerSymbols() { return pointerSymbols; }
	
	public List<Double> getWeights() { return weights; }
	
	/**
	 * Gives a view of <b>this</b> {@link KnowledgeGraphPath} as a
	 * collection {@link KnowledgeGraphPathEdge}
	 * 
	 * @return a list of edges in the path
	 */
	public List<KnowledgeGraphPathEdge> asEdgeList()
	{
		return edges;
	}
	
	/**
	 * Print the path usign a {@link KnowledgeBase} to find the lexicalizations
	 * 
	 * @param kb
	 * @return a stringified representation of <b>this</b> {@link KnowledgeGraphPath}
	 */
	public String toString(KnowledgeBase kb)
	{
		StringBuffer buffer = new StringBuffer();
		for (int i = 0; i < path.size(); i++)
		{
			int position = i % 3;
			switch (position)
			{
				case 0:
					String concept = path.get(i);
					// offsets: convert!
					buffer.
						append(kb == null ? concept : kb.conceptToString(concept)).
						append("[").append(concept).append("]").
						append(" ");
					break;
				case 1:
					// string pointers: do nothing
					buffer.append(path.get(i)).append(":");
					break;
				case 2:
					// weights: append
					buffer.append(path.get(i)).append(" ");
					break;
				default:
					break;
			}
		}
		if (buffer.length() > 0) buffer.deleteCharAt(buffer.length()-1);
		return buffer.toString();
	}

	@Override
	public String toString()
	{
		return toString(null);
	}

	/**
	 * Print the path in a format that can be fed back into the constructor
	 * 
	 * @return a stringified representation of <b>this</b> path that can be fed
	 *         back into the constructor to obtain new instance of <b>this</b>
	 *         {@link KnowledgeGraphPath}
	 */
	public String rawPathToString()
	{
		StringBuffer buffer = new StringBuffer();
		for (String pathElement : path) buffer.append(pathElement).append(" ");

		if (buffer.length() > 0) buffer.deleteCharAt(buffer.length()-1);
		return buffer.toString();
	}

	/**
	 * Check for loops in the path
	 * 
	 * @return whether <b>this</b> {@link KnowledgeGraphPath} has loops
	 */
	public boolean hasLoop()
	{
		// hash of concepts seen
		Set<String> conceptsDone = new HashSet<String>();
	
		// for each element in the path
		for (String concept : concepts)
		{
			// have we seen this offset already?
			if (conceptsDone.contains(concept)) return true;
			conceptsDone.add(concept);
		}
		
		// if we made it so far ... 
		return false;
	}
	
	/**
	 * Converts the flat list of nodes to a structured {@link Graph} object
	 * (unlabeled edges)
	 * 
	 * @return converts <b>this</b> {@link KnowledgeGraphPath} into a directed
	 *         graph
	 */
	public DirectedGraph<String, DefaultEdge> toUnlabeledGraph()
	{
		DirectedGraph<String, DefaultEdge> graph =
			new DefaultDirectedGraph<String, DefaultEdge>(DefaultEdge.class);

		if (path.size() == 1) graph.addVertex(path.get(0));
		
		for (int i = 0; i < path.size(); i++)
		{
			int position = i % 3;
			switch (position)
			{
				case 0:
					// offsets: wait and skip!
					break;
				case 1:
					// even elements are string pointers: add an edge
					
					String src = path.get(i-1);
					String target = path.get(i+2);

					graph.addVertex(src);
					graph.addVertex(target);
					DefaultEdge edge = new DefaultEdge();
					graph.addEdge(src, target, edge);					
					
					break;
				case 2:
					// weights: wait and skip!
					break;
				default:
					break;
			}
		}
		return graph;
	}
	
	/**
	 * Converts the flat list of nodes to a structured {@link Graph} object
	 * (labeled edges)
	 * 
	 * @return converts <b>this</b> {@link KnowledgeGraphPath} into a directed
	 *         graph
	 */
	public DirectedGraph<String, WeightedLabeledEdge> toGraph()
	{
		DirectedGraph<String, WeightedLabeledEdge> graph =
			new DirectedMultigraph<String, WeightedLabeledEdge>(WeightedLabeledEdge.class);

		if (path.size() == 1) graph.addVertex(path.get(0));
		
		for (int i = 0; i < path.size(); i++)
		{
			int position = i % 3;
			switch (position)
			{
				case 0:
					// offsets: wait and skip!
					break;
				case 1:
					// even elements are string pointers: add an edge
					
					String src = path.get(i-1);
					String relation = path.get(i);
					Double weight = Double.valueOf(path.get(i+1));
					String target = path.get(i+2);

					graph.addVertex(src);
					graph.addVertex(target);
					WeightedLabeledEdge edge = new WeightedLabeledEdge(relation, weight);
					graph.addEdge(src, target, edge);					
					
					break;
				case 2:
					// weights: wait and skip!
					break;
				default:
					break;
			}
		}
		return graph;
	}
	
	/**
	 * Given a collection of {@link KnowledgeGraphPath}s, creates a
	 * union {@link DirectedGraph}
	 * 
	 * @param paths
	 * @return takes the union of the input paths to create a graph
	 */
	public static DirectedGraph<String, WeightedLabeledEdge> toUnionGraph(Collection<KnowledgeGraphPath> paths)
	{
		DirectedGraph<String, WeightedLabeledEdge> union =
			new DirectedMultigraph<String, WeightedLabeledEdge>(WeightedLabeledEdge.class);
		
		for (KnowledgeGraphPath path : paths)
		{
			DirectedGraph<String, WeightedLabeledEdge> graph = path.toGraph();
			Graphs.addGraph(union, graph);
		}
		
		return union;
	}
	
	
	/**
	 * Collects all intervening concepts which are found along paths originating 
	 * from more different senses
	 * 
	 * @param paths
	 * @return all intervening concepts which are found along the input paths
	 */
	public static Set<String> getIntersectionConcepts(Collection<KnowledgeGraphPath> paths)
	{
		// map from source to its paths
		Multimap<String, KnowledgeGraphPath> concept2paths = 
			new HashMultimap<String, KnowledgeGraphPath>();
		// map from source to any interviening concept occurring in its path
		SetMultimap<String, String> concept2conceptsInPaths =
			new HashMultimap<String, String>();
		Set<String> intersectionNodes = new HashSet<String>();
		
		for (KnowledgeGraphPath path : paths)
		{
			List<String> pathConcepts = path.getConcepts();
			String sourceConcept = pathConcepts.get(0);
			concept2paths.put(sourceConcept, path);

			for (int pathConcept = 0; pathConcept < pathConcepts.size() - 1; pathConcept++)
			{
				concept2conceptsInPaths.put(sourceConcept,
											pathConcepts.get(pathConcept));
			}
		}
		
		// collect the intersection
		for (String concept1 : concept2paths.keySet())
		{
			Set<String> conceptsInPaths1 = 
				concept2conceptsInPaths.get(concept1);
		
			for (String concept2 : concept2paths.keySet())
			{
				Set<String> conceptsInPaths2 = concept2conceptsInPaths.get(concept2);

				if (concept1.equals(concept2)) continue;

				Set<String> intersection = 
					Sets.intersection(conceptsInPaths1, conceptsInPaths2);
				intersectionNodes.addAll(intersection);
			}
		}
		
		return intersectionNodes;
	}
	
	@Override
	public int hashCode()
	{
		return path != null ? path.hashCode() : 0;
	}
	
	@Override
	public boolean equals(Object obj)
	{
		if (obj instanceof KnowledgeGraphPath)
		{
			KnowledgeGraphPath other = (KnowledgeGraphPath) obj;
			return path.equals(other.path);
		}
		return false;
	}
}
