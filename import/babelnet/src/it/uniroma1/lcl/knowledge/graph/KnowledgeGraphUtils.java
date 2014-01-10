package it.uniroma1.lcl.knowledge.graph;

import it.uniroma1.lcl.jlt.jgrapht.WeightedLabeledEdge;
import it.uniroma1.lcl.jlt.ling.Word;
import it.uniroma1.lcl.jlt.util.Language;
import it.uniroma1.lcl.jlt.util.Maps;
import it.uniroma1.lcl.jlt.util.Pair;
import it.uniroma1.lcl.jlt.util.ScoredItem;
import it.uniroma1.lcl.jlt.util.Strings;
import it.uniroma1.lcl.knowledge.KnowledgeBase;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.jgrapht.DirectedGraph;
import org.jgrapht.Graph;

import com.google.common.collect.HashMultimap;
import com.google.common.collect.Multimap;
import com.google.common.collect.Sets;

import edu.mit.jwi.item.IPointer;

/**
 * Utility methods to work with {@link KnowledgeGraph}s.
 * 
 * @author ponzetto
 * 
 */
public class KnowledgeGraphUtils
{
	private static final Log log = LogFactory.getLog(KnowledgeGraphFactory.class);

	/**
	 * Utility method to intersect two {@link KnowledgeGraph}s.
	 * 
	 * @param graph1
	 * @param graph2
	 * @return a {@link KnowledgeGraph} which is the intersection of the two
	 *         input ones
	 */
	public static KnowledgeGraph intersect(KnowledgeGraph graph1, KnowledgeGraph graph2)
	{
		if (graph1.getKnowledgeBase() != graph2.getKnowledgeBase()) return null;

		Graph<String, WeightedLabeledEdge> g1 = graph1.getGraph();
		Graph<String, WeightedLabeledEdge> g2 = graph2.getGraph();

		Set<String> nodeIntersection = Sets.intersection(g1.vertexSet(), g2.vertexSet());
		Set<WeightedLabeledEdge> edgeIntersection = Sets.intersection(g1.edgeSet(), g2.edgeSet());
		Set<Pair<String, String>> commonEdges = new HashSet<Pair<String, String>>();
		for (WeightedLabeledEdge wle : edgeIntersection)
		{
			String source = g1.getEdgeSource(wle);
			String target = g1.getEdgeTarget(wle);
			commonEdges.add(new Pair<String, String>(source, target));
		}

		// STEP 1: takes the paths
		Set<KnowledgeGraphPath> paths = new HashSet<KnowledgeGraphPath>();
		for (KnowledgeGraph graph : new KnowledgeGraph[] { graph1, graph2 })
		{
			Multimap<String, KnowledgeGraphPath> concept2paths = graph.getConcept2paths();
			for (String concept : concept2paths.keySet())
			{
				PathLoop: for (KnowledgeGraphPath path : concept2paths.get(concept))
				{
					// edge check
					List<String> pathConcepts = path.getConcepts();

					if (pathConcepts.size() == 1)
					{
						String pathConcept = pathConcepts.get(0);
						if (nodeIntersection.contains(pathConcept))
						{
							List<String> pseudoPath = new ArrayList<String>();
							pseudoPath.add(pathConcept);
							paths.add(new KnowledgeGraphPath(pseudoPath, graph1.getKnowledgeBase()));
						}
					}
					else
					{
						for (int i = 0; i < pathConcepts.size() - 1; i++)
						{
							String pathConcept1 = pathConcepts.get(i);
							String pathConcept2 = pathConcepts.get(i + 1);
							Pair<String, String> pseudoEdge = new Pair<String, String>(pathConcept1, pathConcept2);

							if (!commonEdges.contains(pseudoEdge))
							// move to the next path
							continue PathLoop;
						}
						paths.add(path);
					}
				}
			}
		}

		Multimap<String, KnowledgeGraphPath> concept2paths = new HashMultimap<String, KnowledgeGraphPath>();
		Multimap<String, KnowledgeGraphPath> paths2concept = new HashMultimap<String, KnowledgeGraphPath>();

		for (KnowledgeGraphPath path : paths)
		{
			List<String> pathConcepts = path.getConcepts();
			concept2paths.put(pathConcepts.get(0), path);
			paths2concept.put(pathConcepts.get(pathConcepts.size() - 1), path);
		}

		// STEP 2: creates the graph
		DirectedGraph<String, WeightedLabeledEdge> graph = KnowledgeGraphPath.toUnionGraph(concept2paths.values());

		// STEP 3: creates the maps from concepts to nodes
		Multimap<Word, String> sourceWord2concepts = new HashMultimap<Word, String>();
		Multimap<Word, String> sourceWord2concepts1 = graph1.getSourceWord2concepts();
		Multimap<Word, String> sourceWord2concepts2 = graph2.getSourceWord2concepts();

		for (Word sourceWord : sourceWord2concepts1.keySet())
		{
			for (String concept : sourceWord2concepts1.get(sourceWord))
				if (nodeIntersection.contains(concept)) sourceWord2concepts.put(sourceWord, concept);
		}
		for (Word sourceWord : sourceWord2concepts2.keySet())
		{
			for (String concept : sourceWord2concepts2.get(sourceWord))
				if (nodeIntersection.contains(concept)) sourceWord2concepts.put(sourceWord, concept);
		}

		// STEP 4: takes the union of all words
		Set<Word> contextWords = new HashSet<Word>();
		contextWords.addAll(graph1.getContextWords());
		contextWords.addAll(graph2.getContextWords());

		// LAST: creates the knowledge graph
		return new KnowledgeGraph(graph, concept2paths, paths2concept, sourceWord2concepts, contextWords, graph1.getKnowledgeBase());
	}

	/**
	 * Prints the path for each source concept in the {@link KnowledgeGraph}
	 * 
	 * @param graph
	 * @param kb
	 */
	public static void printPaths(KnowledgeGraph graph, KnowledgeBase kb)
	{
		log.info("PRINTING THE PATHS");
		Multimap<String, KnowledgeGraphPath> concept2paths = graph.getConcept2paths();
		for (String concept : concept2paths.keySet())
		{
			StringBuffer pathBuffer = new StringBuffer();
			pathBuffer.append("PRINTING THE PATHS STARTING FROM ").append(concept).append("\n");

			for (KnowledgeGraphPath path : concept2paths.get(concept))
			{
				String pathString = path.toString(kb);
				pathBuffer.append("\t\t").append(pathString).append("\n");
			}
			log.info(pathBuffer);
		}
	}

	/**
	 * Prints information about the scoring of a {@link KnowledgeGraph} with a
	 * specific scorer
	 * 
	 * @param simpleGraph
	 * @param fullGraph
	 * @param scorer
	 * @param kb
	 */
	public static void printScoring(KnowledgeGraph graph, KnowledgeGraphScorer scorer, KnowledgeBase kb)
	{
		StringBuffer buffer = new StringBuffer();
		Collection<String> concepts = graph.getConceptsForSourceWords();

		// scores the senses in the graph
		Map<String, Double> scores = scorer.score(graph, concepts);
		Map<String, Double> sortedScores = Maps.sortByValue(scores);
		for (String concept : sortedScores.keySet())
		{
			buffer.append("\n    OFFSET = ").append(concept).append(" SENSE = ").append(kb.conceptToString(concept)).append(" SCORER => ").append(scorer).append(" SCORE = ")
					.append(sortedScores.get(concept));
		}
		buffer.append("\n    ==============================");

		log.info(buffer);
	}

	/**
	 * Prints information about the degree of a {@link KnowledgeGraph} with a
	 * specific scorer
	 * 
	 * @param simpleGraph
	 * @param kb
	 */
	public static void printDegree(KnowledgeGraph simpleGraph, KnowledgeBase kb)
	{
		StringBuffer buffer = new StringBuffer();
		Collection<String> concepts = simpleGraph.getConceptsForSourceWords();
		DirectedGraph<String, WeightedLabeledEdge> graph = simpleGraph.getGraph();

		for (String concept : concepts)
		{
			double weightedSum = 0.0;
			int degree = graph.outDegreeOf(concept);
			Multimap<IPointer, ScoredItem<String>> related = kb.getRelatedConceptsMap(concept);

			buffer.append("\n    OFFSET = ").append(concept).append(" SENSE = ").append(kb.conceptToString(concept)).append("\n\n      OUTGOING EDGES\n");

			for (WeightedLabeledEdge edge : graph.outgoingEdgesOf(concept))
			{
				weightedSum += edge.getWeight();

				String src = graph.getEdgeSource(edge);
				String target = graph.getEdgeTarget(edge);

				buffer.append("\n      ").append(kb.conceptToString(src)).append(":").append(edge.getLabel()).append("[").append(edge.getWeight()).append("]:").append(kb.conceptToString(target));
			}
			buffer.append("\n    ==============================");
			buffer.append("\n    DEGREE: ").append(degree).append("/").append(related.size()).append("=").append(Strings.format((double) degree / (double) related.size()))
					.append(" -- WEIGHTED SUM: ").append(Strings.format(weightedSum)).append("/").append(related.size()).append("=")
					.append(Strings.format((double) weightedSum / (double) related.size()));
			buffer.append("\n    ==============================");
		}

		log.info(buffer);
	}

	/**
	 * Prints information about words, senses and their scores in a
	 * {@link KnowledgeGraph}
	 * 
	 * @param graph
	 * @param kb
	 */
	public static void printLexicon(KnowledgeGraph graph, KnowledgeBase kb)
	{
		StringBuffer buffer = new StringBuffer();

		// for each word of interest
		for (Word word : graph.getSourceWords())
		{
			buffer.append("\n  WORD = ").append(word);

			// its senses
			Collection<String> concepts = graph.getConceptsForSourceWord(word);

			for (String concept : concepts)
			{
				Multimap<Language, Word> lexicalizations = kb.getConceptWordsByLanguage(concept);

				buffer.append("\n    OFFSET = ").append(concept).append(" SENSE = ").append(kb.conceptToString(concept)).append("\n\n      LEXICALIZATION\n");

				for (Language language : lexicalizations.keySet())
				{
					for (Word lexicalization : lexicalizations.get(language))
					{
						buffer.append("\n      ").append(language).append(":").append(lexicalization);
					}
				}
				buffer.append("\n      ==============================");
			}
		}
		log.info(buffer);
	}

	public static void analyseContext(Collection<Word> targetWords, Collection<Word> contextWords, KnowledgeBase kb) throws IOException
	{
		KnowledgeGraphFactory disGraphfactory = KnowledgeGraphFactory.getInstance(kb);
		KnowledgeGraph graph = disGraphfactory.getKnowledgeGraph(targetWords, contextWords);

		for (KnowledgeGraphScorer scorer : KnowledgeGraphScorer.values())
		{
			switch (scorer)
			{
				case SUM_INVERSE_PATH_LENGTH:
				case SUM_PATH_PROBABILITY:
				case DEGREE:
					printScoring(graph, scorer, kb);
					break;
				case PAGERANK:
					// for pagerank we build a graph with edges also from
					// context words to the target words
					KnowledgeGraph fullGraph = disGraphfactory.getKnowledgeGraph(contextWords, contextWords);
					printScoring(fullGraph, scorer, kb);
					break;
				case UPPER_BOUND:
					// do nothing
					break;
				default:
					throw new RuntimeException("Unknown disambiguation graph scorer");
			}
		}

		String outFile = "tmp/" + kb.name().toLowerCase() + "_path-test.dot";
		log.info("DUMPING THE GRAPH TO: " + outFile);
		graph.saveToDot(outFile);

		printPaths(graph, kb);
		printDegree(graph, kb);
		printLexicon(graph, kb);

	}

	public static void disambiguate(Collection<Word> words,
									KnowledgeBase kb,
									KnowledgeGraphScorer scorer) throws IOException
	{
		KnowledgeGraphFactory factory = KnowledgeGraphFactory.getInstance(kb);
		KnowledgeGraph kGraph = factory.getKnowledgeGraph(words);
		Map<String, Double> scores = scorer.score(kGraph);
		for (String concept : scores.keySet())
		{
			double score = scores.get(concept);
			for (Word word : kGraph.wordsForConcept(concept))
				word.addLabel(concept, score);
		}
		for (Word word : words)
		{
			System.out.println("\n\t" + word.getWord() + " -- ID " +
							   word.getId() + " => SENSE DISTRIBUTION: ");
			for (ScoredItem<String> label : word.getLabels())
			{
				System.out.println("\t  [" + label.getItem() + "]:" +
								   Strings.format(label.getScore()));
			}
		}
	}

	public static void acl12demo() throws IOException
	{
		List<Word> sentence =
			Arrays.asList(new Word[] {
				new Word("bank", 'n', Language.EN),
				new Word("bonus", 'n', Language.EN),
				new Word("pay", 'v', Language.EN),
				new Word("stock", 'n', Language.EN)
		});
		disambiguate(sentence, KnowledgeBase.BABELNET, KnowledgeGraphScorer.DEGREE);
	}

	public static void simpleTest() throws IOException
	{
		Word w1 = new Word("bank", 'n');
		Word w2 = new Word("finance", 'n');
		Word w3 = new Word("account", 'n');

		// UNCOMMENT TO SELECT THE KB
		KnowledgeBase kb = null;
		// kb = KnowledgeBase.WORDNET;
		kb = KnowledgeBase.BABELNET;

		List<Word> sourceWords = Collections.singletonList(w1);
		List<Word> contextWords = Arrays.asList(w1, w2, w3);

		analyseContext(sourceWords, contextWords, kb);
	}

	/**
	 * Use to test the search
	 * 
	 * @param args
	 */
	public static void main(String[] args)
	{
		try
		{
			acl12demo();
			// simpleTest();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
}
