package it.uniroma1.lcl.knowledge.graph;

import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import it.uniroma1.lcl.jlt.graphviz.DotColors;
import it.uniroma1.lcl.jlt.graphviz.DotOptions;
import it.uniroma1.lcl.jlt.jgrapht.WeightedLabeledEdge;
import it.uniroma1.lcl.jlt.ling.Word;
import it.uniroma1.lcl.jlt.util.Pair;
import it.uniroma1.lcl.knowledge.KnowledgeBase;

import org.jgrapht.DirectedGraph;
import org.jgrapht.ext.VertexNameProvider;

import com.google.common.collect.HashMultimap;
import com.google.common.collect.Iterables;
import com.google.common.collect.Multimap;

/**
 * Class representing a knowledge graph.
 * 
 * @author ponzetto
 *
 */
public class KnowledgeGraph
{
	/**
	 * The actual graph
	 */
	private DirectedGraph<String, WeightedLabeledEdge> graph;
	
	/**
	 * Map from senses to the paths starting with that sense
	 */
	private final Multimap<String, KnowledgeGraphPath> concept2paths;	
	
	/**
	 * Map from senses to the paths ending with that sense
	 */
	private final Multimap<String, KnowledgeGraphPath> paths2concept;
	
	/**
	 * Map from source {@link Word}s to the nodes denoting their meanings in
	 * the graph
	 */
	private final Multimap<Word, String> sourceWord2concepts;

	/**
	 * Map from concepts to their lexicalizations (source words only)
	 */
	private final Multimap<String, Word> concepts2sourceWords;
	
	/**
	 * The words of the context
	 */
	private final Collection<Word> contextWords;
	
	/**
	 * The {@link KnowledgeBase} this graph is created from
	 */
	private final KnowledgeBase kb;
	
	/**
 	 * Creates a new instance of {@link KnowledgeGraph}
	 * 
	 * @param graph
	 * @param concept2paths
	 * @param paths2concept
	 * @param sourceWord2concepts
	 * @param contextWords
	 * @param kb
	 */
	public KnowledgeGraph(DirectedGraph<String, WeightedLabeledEdge> graph,
						  Multimap<String, KnowledgeGraphPath> concept2paths,
						  Multimap<String, KnowledgeGraphPath> paths2concept,
						  Multimap<Word, String> sourceWord2concepts,
						  Collection<Word> contextWords,
						  KnowledgeBase kb)
	{
		this.graph = graph;
		this.concept2paths = concept2paths;
		this.paths2concept = paths2concept;

		this.sourceWord2concepts = sourceWord2concepts;
		this.concepts2sourceWords = new HashMultimap<String, Word>();
		for (Word word : sourceWord2concepts.keySet())
			for (String concept : sourceWord2concepts.get(word))
				concepts2sourceWords.put(concept, word);
				
		this.contextWords = contextWords;
		this.kb = kb;
		
	}
	
	public DirectedGraph<String, WeightedLabeledEdge> getGraph()
	{ return graph; }
	
	public Collection<String> getConceptsForSourceWord(Word word)
	{ return sourceWord2concepts.get(word); }

	public Collection<String> getConceptsForSourceWords()
	{ return sourceWord2concepts.values(); }
	
	public Multimap<String, KnowledgeGraphPath> getPaths2concept()
	{ return paths2concept; }
	
	public Multimap<String, KnowledgeGraphPath> getConcept2paths()
	{ return concept2paths; }
	
	public Set<Word> getSourceWords()
	{ return sourceWord2concepts.keySet(); }
	
	public Multimap<Word, String> getSourceWord2concepts()
	{ return sourceWord2concepts; }
	
	public Collection<Word> getContextWords()
	{ return contextWords; }
	
	public KnowledgeBase getKnowledgeBase()
	{ return kb; }
	
	public Collection<Word> wordsForConcept(String concept)
	{
		return concepts2sourceWords.get(concept);
	}
	
	/**
	 * Prints the input graph to an external file in DOT format, highlighting
	 * the senses of the input words, using a {@link KnowledgeBase} to
	 * lexicalize the nodes appropriately
	 * 
	 * @param kb
	 * @param file
	 */
	public void saveToDot(final String file)
	{
		Multimap<String, Pair<DotOptions, String>> options = 
			new HashMultimap<String, Pair<DotOptions, String>>();
		
		
		List<DotColors> colorsList = Arrays.asList(DotColors.values());
		Collections.shuffle(colorsList);
		// iteratory to cycle indefinitely over the colors
		Iterator<DotColors> colorIterator = Iterables.cycle(colorsList).iterator();

		final Set<String> sourceWordConcepts = new HashSet<String>();
		final Set<String> contextWordConcepts = new HashSet<String>();
		for (Word word : sourceWord2concepts.keySet())
		{
			// all senses of a word receive the same color
			String colorForWord = colorIterator.next().name().toLowerCase();
			for (String concept : sourceWord2concepts.get(word))
			{
				sourceWordConcepts.add(concept);
				options.put(concept,
							new Pair<DotOptions, String>(DotOptions.FILLCOLOR,
														 colorForWord));
			}
		}

		if (contextWords != null)
		{
			String contextWordColor = colorIterator.next().name().toLowerCase();
			for (Word word : contextWords)
			{
				for (String concept : kb.getConcepts(word))
				{
					contextWordConcepts.add(concept);
					options.put(concept,
							new Pair<DotOptions, String>(DotOptions.FILLCOLOR,
														 contextWordColor));
				}
			}
		}
		
		// trasforms the concepts into appropriate string representations
		final Map<String, String> vertexToString = new HashMap<String, String>();
		for (String concept : graph.vertexSet())
		{
			String stringified = kb.conceptToString(concept);
			
			if (sourceWordConcepts.contains(concept))
				vertexToString.put(concept, "[S] " + stringified);
			else if (contextWordConcepts.contains(concept))
				vertexToString.put(concept, "[C] " + stringified);
			else
				vertexToString.put(concept, stringified);
		}
		
		for (String concept : vertexToString.keySet())
		{
			options.put(concept,
						new Pair<DotOptions, String>(DotOptions.SIZE,
													 (vertexToString.get(concept).toCharArray().length)*10+""));
		}
		
		// saves the graph
		it.uniroma1.lcl.jlt.jgrapht.Graphs.saveDotFile(
			graph, file,
			options,
			new HashMultimap<WeightedLabeledEdge, Pair<DotOptions, String>>(),
			new VertexNameProvider<String>()
			{
				public String getVertexName(String arg0)
				{ return vertexToString.get(arg0); }
			}
		);
	}
}