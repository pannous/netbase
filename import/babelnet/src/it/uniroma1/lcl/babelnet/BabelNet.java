package it.uniroma1.lcl.babelnet;

import it.uniroma1.lcl.babelnet.iterators.BabelLexiconIterator;
import it.uniroma1.lcl.babelnet.iterators.BabelOffsetIterator;
import it.uniroma1.lcl.babelnet.iterators.BabelSynsetIterator;
import it.uniroma1.lcl.jlt.util.IntegerCounter;
import it.uniroma1.lcl.jlt.util.Language;
import it.uniroma1.lcl.jlt.util.ScoredItem;
import it.uniroma1.lcl.jlt.util.Triple;
import it.uniroma1.lcl.jlt.wordnet.WordNet;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.index.Term;
import org.apache.lucene.search.BooleanClause;
import org.apache.lucene.search.BooleanClause.Occur;
import org.apache.lucene.search.BooleanQuery;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.MatchAllDocsQuery;
import org.apache.lucene.search.ScoreDoc;
import org.apache.lucene.search.TermQuery;
import org.apache.lucene.search.TopDocs;
import org.apache.lucene.store.Directory;
import org.apache.lucene.store.SimpleFSDirectory;

import com.google.common.collect.HashMultimap;
import com.google.common.collect.Multimap;

import edu.mit.jwi.item.POS;

/**
 * A class to programmatically access BabelNet.
 *
 * @author navigli, ponzetto
 *
 */
public class BabelNet
{
	private static final Log log = LogFactory.getLog(BabelNet.class);

	static private BabelNet instance;

	public static final String SEPARATOR = ":";

	private static final int MAXIMUM_NUMBER_OF_SYNSETS = 500;

	private static final boolean USE_REDIRECTION_SENSES =
		BabelNetConfiguration.getInstance().getBabelNetUseRedirectionSenses();

	private static final boolean USE_IMAGE_FILTER =
			BabelNetConfiguration.getInstance().isBadImageFilterActive();

	/**
	 * Objects for searching the Lucene index
	 */
	private final IndexSearcher lexicon;
	private final IndexSearcher dictionary;
	private final IndexSearcher glosses;
	private final IndexSearcher graph;

	/**
	 * The private constructor used to initialize the BabelNet indexes
	 *
	 * @throws IOException
	 */
	private BabelNet() throws IOException
	{
		BabelNetConfiguration config = BabelNetConfiguration.getInstance();

		String lexiconFile = config.getBabelNetLexiconIndexDir();
		String dictionaryFile = config.getBabelNetDictIndexDir();
		String glossFile = config.getBabelNetGlossIndexDir();
		String graphFile = config.getBabelNetGraphIndexDir();

		Directory lexiconDir = new SimpleFSDirectory(new File(lexiconFile));
		Directory dictionaryDir = new SimpleFSDirectory(new File(dictionaryFile));
		Directory glossDir = new SimpleFSDirectory(new File(glossFile));
		Directory graphDir = new SimpleFSDirectory(new File(graphFile));

		log.info("OPENING BABEL LEXICON FROM: " + lexiconFile);
		log.info("OPENING BABEL DICTIONARY FROM: " + dictionaryFile);
		log.info("OPENING BABEL GLOSSES FROM: " + glossFile);
		log.info("OPENING BABEL GRAPH FROM: " + graphFile);

		// apre gli indici e li tiene aperti
		this.lexicon = new IndexSearcher(lexiconDir, true);
		this.dictionary = new IndexSearcher(dictionaryDir, true);
		this.glosses = new IndexSearcher(glossDir, true);
		this.graph = new IndexSearcher(graphDir, true);
	}


	/**
	 * Used to access {@link BabelNet}
	 *
	 * @return an instance of {@link BabelNet}
	 */
	public static synchronized BabelNet getInstance()
	{
		try
		{
			if (instance == null) instance = new BabelNet();
			return instance;
		}
		catch (IOException e)
		{
			throw new RuntimeException("\nCould not init BabelNet: " + e.getMessage()+"\n" +
					"Set the value of \"babelnet.dir=\" in the file: config/babelnet.var.properties");
		}
	}

	/**
	 * Given a word, returns the senses for the word. Assumes the word is a
	 * noun.
	 *
	 * @param language
	 *            the language of the input word.
	 * @param word
	 *            the word whose senses are to be retrieved.
	 * @return the senses of the word.
	 *
	 */
	public List<BabelSense> getSenses(Language language, String word) throws IOException
	{
		return getSenses(language, word, POS.NOUN);
	}

	/**
	 * Given a word, returns the senses for the word.
	 *
	 * @param language
	 *            the language of the input word.
	 * @param word
	 *            the word whose senses are to be retrieved.
	 * @param pos
	 *            the PoS of the word.
	 * @return the senses of the word.
	 *
	 */
	public List<BabelSense> getSenses(Language language, String word, POS pos) throws IOException
	{
		return getSenses(language, word, pos, USE_REDIRECTION_SENSES);
	}

	/**
	 * Given a word, returns the senses for the word.
	 *
	 * @param language
	 *            the language of the input word.
	 * @param word
	 *            the word whose senses are to be retrieved.
	 * @param pos
	 *            the PoS of the word.
	 * @param allowedSources
	 *            the {@link BabelSenseSource}s that can be used to look up
	 *            the possible senses of the input word.
	 * @return the senses of the word.
	 *
	 */
	public List<BabelSense> getSenses(Language language, String word, POS pos,
									  BabelSenseSource... allowedSources) throws IOException
	{
		return getSenses(language, word, pos, USE_REDIRECTION_SENSES, allowedSources);
	}

	/**
	 * Returns the version of BabelNet
	 *
	 * @return version constant
	 */
	public BabelVersion getVersion()
	{
		try
		{
			TopDocs docs = dictionary.search(new MatchAllDocsQuery(BabelNetIndexField.VERSION.toString()),1);
			if (docs.totalHits == 0) return BabelVersion.PRE_2_0;
			else
			{
			    Document doc = dictionary.doc(docs.scoreDocs[0].doc);
			    String versionString = doc.get(BabelNetIndexField.VERSION.toString());
				return BabelVersion.valueOf(versionString);
			}
		}
		catch(IOException e)
		{
			return BabelVersion.UNKNOWN;
		}
	}

	/**
	 * Given a word, returns the senses for the word.
	 *
	 * @param language
	 *            the language of the input word.
	 * @param word
	 *            the word whose senses are to be retrieved.
	 * @param pos
	 *            the PoS of the word.
	 * @param includeRedirections
	 *            whether to include synsets senses of the word
	 *            which are redirections in Wikipedia.
	 * @param allowedSources
	 *            the {@link BabelSenseSource}s that can be used to look up
	 *            the possible senses of the input word.
	 * @return the senses of the word.
	 *
	 */
	public List<BabelSense> getSenses(Language language, String word,
									  POS pos, boolean includeRedirections,
									  BabelSenseSource... allowedSources) throws IOException
	{
		String langWord =
			new StringBuffer(language.toString()).
				append(SEPARATOR).append(word.toLowerCase()).toString();

		BooleanQuery q = new BooleanQuery();
		q.add(new BooleanClause(new TermQuery(
				new Term(BabelNetIndexField.LANGUAGE_LEMMA.toString(),
						langWord)),
						Occur.MUST));
		if (pos != null)
			q.add(new BooleanClause(new TermQuery(
				new Term(BabelNetIndexField.POS.toString(),
						Character.toString(pos.getTag()))),
						Occur.MUST));

		// interroga l'indice e restituisce la lista
		TopDocs docs = dictionary.search(q, MAXIMUM_NUMBER_OF_SYNSETS);

	    List<BabelSense> senses = new ArrayList<BabelSense>();

		for (ScoreDoc scoreDoc : docs.scoreDocs)
		{
		    Document doc = dictionary.doc(scoreDoc.doc);
		    BabelSynset babelSynset = getBabelSynsetFromDocument(doc);
		    List<BabelSense> allSenses = babelSynset.getSenses(language, word);
		    List<BabelSenseSource> allowedSourceList = Arrays.asList(allowedSources);

		    for (BabelSense sense : allSenses)
		    {
		    	// check for specific sources we are interested in
		    	if (!allowedSourceList.isEmpty() && !allowedSourceList.contains(sense.getSource()))
		    		continue;

		    	// check whether we need to consider senses of redirections
		    	if (includeRedirections || sense.getSource() != BabelSenseSource.WIKIRED)
		    		senses.add(sense);
		    }
		}

		return senses;
	}

	/**
	 * Given a word, returns the senses for the word. Assumes the word is a
	 * noun.
	 *
	 * @param language
	 *            the language of the input word.
	 * @param word
	 *            the word whose senses are to be retrieved.
	 * @return the senses of the word.
	 *
	 */
	public List<BabelSynset> getSynsets(Language language, String word) throws IOException
	{
		return getSynsets(language, word, POS.NOUN);
	}

	/**
	 * Given a word, returns the senses for the word.
	 *
	 * @param language
	 *            the language of the input word.
	 * @param word
	 *            the word whose senses are to be retrieved.
	 * @param pos
	 *            the PoS of the word.
	 * @return the senses of the word.
	 *
	 */
	public List<BabelSynset> getSynsets(Language language, String word, POS pos) throws IOException
	{
		return getSynsets(language, word, pos, USE_REDIRECTION_SENSES);
	}

	/**
	 * Given a word, returns the senses for the word.
	 *
	 * @param language
	 *            the language of the input word.
	 * @param word
	 *            the word whose senses are to be retrieved.
	 * @param pos
	 *            the PoS of the word.
	 * @param allowedSources
	 *            the {@link BabelSenseSource}s that can be used to look up
	 *            the possible senses of the input word.
	 * @return the senses of the word.
	 *
	 */
	public List<BabelSynset> getSynsets(Language language, String word, POS pos,
										BabelSenseSource... allowedSources) throws IOException
	{
		return getSynsets(language, word, pos, USE_REDIRECTION_SENSES, allowedSources);
	}

	/**
	 * Given a word, returns the senses for the word.
	 *
	 * @param language
	 *            the language of the input word.
	 * @param word
	 *            the word whose senses are to be retrieved.
	 * @param pos
	 *            the PoS of the word.
	 * @param includeRedirections
	 *            whether to include synsets with senses of the word which are
	 *            only redirections in Wikipedia.
	 * @param allowedSources
	 *            the {@link BabelSenseSource}s that can be used to look up the
	 *            possible senses of the input word.
	 * @return the senses of the word.
	 */
	public List<BabelSynset> getSynsets(Language language, String word,
										POS pos, boolean includeRedirections,
										BabelSenseSource... allowedSources) throws IOException
	{
		String langWord =
			new StringBuffer(language.toString()).
				append(SEPARATOR).append(word.toLowerCase()).toString();

		BooleanQuery q = new BooleanQuery();
		q.add(new BooleanClause(new TermQuery(
				new Term(BabelNetIndexField.LANGUAGE_LEMMA.toString(),
						langWord)),
						Occur.MUST));
		if (pos != null)
			q.add(new BooleanClause(new TermQuery(
				new Term(BabelNetIndexField.POS.toString(),
						Character.toString(pos.getTag()))),
						Occur.MUST));

		// interroga l'indice e restituisce la lista
		TopDocs docs = dictionary.search(q, MAXIMUM_NUMBER_OF_SYNSETS);

	    List<BabelSynset> synsets = new ArrayList<BabelSynset>();

		for (ScoreDoc scoreDoc : docs.scoreDocs)
		{
		    Document doc = dictionary.doc(scoreDoc.doc);

		    BabelSynset babelSynset = getBabelSynsetFromDocument(doc);
		    synsets.add(babelSynset);
		}

		// check whether to include specific types of senses
		if (allowedSources.length > 0)
		{
			Set<BabelSynset> remove = new HashSet<BabelSynset>();
			List<BabelSenseSource> allowedSourceList = Arrays.asList(allowedSources);

			for (BabelSynset synset : synsets)
			{
				boolean toRemove = true;
				List<BabelSense> senses = synset.getSenses(language, word);
				for (BabelSense sense : senses)
				{
					BabelSenseSource source = sense.getSource();
					if (allowedSourceList.contains(source))
					{
						toRemove = false;
						break;
					}
				}
				if (toRemove) remove.add(synset);
			}
			synsets.removeAll(remove);
		}

		// check whether to include redirection senses
		if (!includeRedirections)
		{
			// do not consider if it comes only from a redirection
			Set<BabelSynset> remove = new HashSet<BabelSynset>();
			for (BabelSynset synset : synsets)
			{
				Set<BabelSenseSource> sources = new HashSet<BabelSenseSource>();
				List<BabelSense> senses = synset.getSenses(language, word);
				for (BabelSense sense : senses)
				{
					BabelSenseSource source = sense.getSource();
					switch (source)
					{
						case WIKIRED:
							// counts as a redirection only if it has a
							// sense label
							String lemma = sense.getLemma();
							String simpleLemma = sense.getSimpleLemma();
							if (!simpleLemma.equalsIgnoreCase(lemma))
								sources.add(sense.getSource());
							break;

						default:
							sources.add(sense.getSource());
							break;
					}
				}
				if (sources.size() == 1 && sources.iterator().next() == BabelSenseSource.WIKIRED)
					remove.add(synset);
			}
			synsets.removeAll(remove);
		}

		return synsets;
	}

	/**
	 * Gets a full-fledged {@link BabelSynset} from a concept identifier (Babel
	 * synset ID).
	 *
	 * @param id
	 *            the Babel synset ID for a specific concept
	 * @return an instance of a {@link BabelSynset} from a concept ID
	 * @throws IOException
	 */
	public BabelSynset getSynsetFromId(String id) throws IOException
	{
		TermQuery q = new TermQuery(new Term(BabelNetIndexField.ID.toString(), id));

		// interroga l'indice e restituisce il synset se esiste
		TopDocs docs = dictionary.search(q, 1);

		// nessun synset trovato
		if (docs.totalHits == 0) return null;

		// restituisce il synset
		Document doc = dictionary.doc(docs.scoreDocs[0].doc);
		return getBabelSynsetFromDocument(doc);
	}

	/**
	 * Gets a full-fledged {@link BabelSynset} from a {@link Document}
	 *
	 * @param doc
	 *            a Lucene {@link Document} record for a certain Babel synset
	 * @return an instance of a {@link BabelSynset} from a {@link Document}
	 */
	public static BabelSynset getBabelSynsetFromDocument(Document doc)
	{
	    String id = doc.get(BabelNetIndexField.ID.toString());
	    BabelSynsetSource synsetSource =
	    	BabelSynsetSource.valueOf(doc.get(BabelNetIndexField.SOURCE.toString()));
	    BabelSynsetType synsetType =
	    	BabelSynsetType.valueOf(doc.get(BabelNetIndexField.TYPE.toString()));
	    String mainSense = doc.get(BabelNetIndexField.MAIN_SENSE.toString());
	    POS pos =
	    	WordNet.getPOSfromChar(
	    		doc.get(BabelNetIndexField.POS.toString()));
	    List<String> wnOffsets =
	    	Arrays.asList(doc.getValues(BabelNetIndexField.WORDNET_OFFSET.toString()));
	    List<String> translationMappings =
    		Arrays.asList(doc.getValues(BabelNetIndexField.TRANSLATION_MAPPING.toString()));

	    String[] imageNames = doc.getValues(BabelNetIndexField.IMAGE.toString());
	    List<BabelImage> images = new ArrayList<BabelImage>();
	    for (String imageName : imageNames)
	    {
	    	BabelImage babelImage = new BabelImage(imageName);
	    	if (!USE_IMAGE_FILTER || !babelImage.isBadImage())
	    		images.add(babelImage);
	    }

	    String[] categoryNames = doc.getValues(BabelNetIndexField.CATEGORY.toString());
	    List<BabelCategory> categories = new ArrayList<BabelCategory>();
	    for (String categoryName : categoryNames)
	    	categories.add(BabelCategory.fromString(categoryName));

	    String[] lemmas = doc.getValues(BabelNetIndexField.LEMMA.toString());
	    String[] lemmaSources = doc.getValues(BabelNetIndexField.LEMMA_SOURCE.toString());
	    String[] lemmaLanguages = doc.getValues(BabelNetIndexField.LEMMA_LANGUAGE.toString());
	    String[] lemmaWeights = doc.getValues(BabelNetIndexField.LEMMA_WEIGHT.toString());
	    String[] lemmaSensekeys = doc.getValues(BabelNetIndexField.LEMMA_SENSEKEY.toString());

	    List<BabelSense> synsetSenses = new ArrayList<BabelSense>();
	    BabelSynset babelSynset =
	    	new BabelSynset(
	    			id,
	    			pos,
	    			synsetSource,
	    			wnOffsets,
	    			synsetSenses,
	    			translationMappings,
	    			images,
	    			categories,
	    			mainSense,
	    			synsetType);

	    for (int k = 0; k < lemmas.length; k++)
	    {
	    	String lemma = lemmas[k];
	    	Language lemmaLanguage = Language.valueOf(lemmaLanguages[k]);
	    	String lemmaWeight = lemmaWeights[k];
	    	BabelSenseSource lemmaSource = BabelSenseSource.valueOf(lemmaSources[k]);
	    	String lemmaSenseskey = lemmaSensekeys[k];
	    	String[] senseOffsetTriples = lemmaSenseskey.split("\t");

	    	for(int j = 0; j < senseOffsetTriples.length; j += 3)
			{
	    		String lemmaSensekey = senseOffsetTriples[j];
	    		String wordnetOffset = senseOffsetTriples.length == 1 ? null : senseOffsetTriples[j+1];

	    		// note: position within the WordNet synset is in hex format (always "1" for Wikipedia titles)
	    		String position = senseOffsetTriples.length == 1 ? "1" : senseOffsetTriples[j+2];

	    		BabelSense sense =
					new BabelSense(
							lemmaLanguage,
							lemma,
							pos,
							lemmaSource,
							lemmaSensekey,
							wordnetOffset,
							Integer.valueOf(String.valueOf(position), 16),
							lemmaWeight,
							babelSynset);

	    		synsetSenses.add(sense);
			}
	    }

	    return babelSynset;
	}

	/**
	 * Get the {@link BabelSynset}s corresponding to an input WordNet offset
	 *
	 * @param offset
	 *            a WordNet offset
	 * @return a {@link List} of {@link BabelSynset}s corresponding to the input
	 *         WordNet offset
	 * @throws IOException
	 */
	public List<BabelSynset> getBabelSynsetsFromWordNetOffset(String offset) throws IOException
	{
		TermQuery q = new TermQuery(new Term(BabelNetIndexField.WORDNET_OFFSET.toString(), offset));

		// interroga l'indice e restituisce il synset se esiste
		TopDocs docs = dictionary.search(q, MAXIMUM_NUMBER_OF_SYNSETS);

		// nessun synset trovato
		if (docs.totalHits == 0) return null;

		// restituisce i synset
	    List<BabelSynset> synsets = new ArrayList<BabelSynset>();

		for (ScoreDoc scoreDoc : docs.scoreDocs)
		{
		    Document doc = dictionary.doc(scoreDoc.doc);

		    BabelSynset babelSynset = getBabelSynsetFromDocument(doc);
		    synsets.add(babelSynset);
		}

		return synsets;
	}

	/**
	 * Given an exact Wikipedia title, returns its {@link BabelSynset}s.
	 *
	 * @param language
	 *            the language of the input Wikipedia title.
	 * @param title
	 *            the language of the Wikipedia page.
	 * @param pos
	 *            the PoS of the Wikipedia title.
	 * @return the set of {@link BabelSynset}s associated to the given Wikipedia title
	 */
	public Set<BabelSynset> getSynsetsFromWikipediaTitle(Language language, String title, POS pos) throws IOException
			{

		title = title.replaceAll(" ", "_");

		BooleanQuery q = new BooleanQuery();
		q.add(new BooleanClause(new TermQuery(new Term(BabelNetIndexField.LEMMA.toString(), title)), Occur.MUST));

		if (pos != null)
			q.add(new BooleanClause(new TermQuery(new Term(BabelNetIndexField.POS.toString(),Character.toString(pos.getTag()))), Occur.MUST));

		// interroga l'indice e restituisce la lista
		TopDocs docs = dictionary.search(q, MAXIMUM_NUMBER_OF_SYNSETS);

		Set<BabelSynset> synsets = new HashSet<BabelSynset>();

		for (ScoreDoc scoreDoc : docs.scoreDocs)
		{
			Document doc = dictionary.doc(scoreDoc.doc);

			Field[] lemmas = doc.getFields(BabelNetIndexField.LEMMA.toString());
			Field[] lemmasLanguages = doc.getFields(BabelNetIndexField.LEMMA_LANGUAGE.toString());
			Field[] lemmasSource = doc.getFields(BabelNetIndexField.LEMMA_SOURCE.toString());

			for(int i=0; i<lemmas.length; i++)
				if(Language.valueOf(lemmasLanguages[i].stringValue()) == language && lemmas[i].stringValue().equals(title) && BabelSynsetSource.valueOf(lemmasSource[i].stringValue()) != BabelSynsetSource.WN)
					synsets.add(getBabelSynsetFromDocument(doc));
		}

		return synsets;
	}

	/**
	 * Given a Babel id, collects the successors of the concept denoted by the
	 * id
	 *
	 * @param concept
	 *            a concept identifier (babel synset ID)
	 * @return a stringified representation of the edges departing from the
	 *         Babel synset denoted by the input id
	 * @throws IOException
	 */
	public List<String> getSuccessors(String concept) throws IOException
	{
		TermQuery q = new TermQuery(new Term(BabelNetIndexField.ID.toString(), concept));

		TopDocs docs = graph.search(q, 1);
		Document doc = graph.doc(docs.scoreDocs[0].doc);

		String successors = doc.get(BabelNetIndexField.RELATION.toString());

		return Arrays.asList(successors.split("\t"));
	}

	/**
	 * Given a Babel id, collects the successor {@link BabelNetGraphEdge} of the
	 * concept denoted by the id
	 *
	 * @param concept
	 *            a concept identifier (babel synset ID)
	 * @return the edges departing from the Babel synset denoted by the input id
	 * @throws IOException
	 */
	public List<BabelNetGraphEdge> getSuccessorEdges(String concept) throws IOException
	{
		List<BabelNetGraphEdge> related = new ArrayList<BabelNetGraphEdge>();
		List<String> relatedEdgeStrings = getSuccessors(concept);

		for (String relatedEdgeString : relatedEdgeStrings)
		{
			if (relatedEdgeString.isEmpty()) continue;
			BabelNetGraphEdge edge = BabelNetGraphEdge.fromString(relatedEdgeString);
			related.add(edge);
		}

		return related;
	}

	/**
	 * Gets translations of an input word.
	 *
	 * @param language
	 *            the language of the input word.
	 * @param word
	 *            the word whose senses are to be retrieved.
	 * @return the translations of the input words in different languages, each
	 *         weighted by the number of times the input word was translated as
	 *         such
	 * @throws IOException
	 */
	public Multimap<Language, ScoredItem<String>> getTranslations(Language language, String word) throws IOException
	{
		Map<Language, IntegerCounter<String>> traslationCounters =
				new HashMap<Language, IntegerCounter<String>>();

		List<BabelSynset> synsets = getSynsets(language, word);
		for (BabelSynset synset : synsets)
		{
			// helps considering only once multiple source-target pairs in a
			// specific language with the same lemma (e.g., overblown
			// redirections)
			Set<Triple<String, String, Language>> translationsDone =
				new HashSet<Triple<String,String,Language>>();

			Multimap<BabelSense, BabelSense> translations = synset.getTranslations();
			List<BabelSense> senses = synset.getSenses(language, word);

			for (BabelSense sense : senses)
			{
				String senseLemma = sense.getSimpleLemma().toLowerCase();
				Collection<BabelSense> senseTranslations = translations.get(sense);
				for (BabelSense senseTranslation : senseTranslations)
				{
					Language translationLanguage = senseTranslation.getLanguage();
					if (translationLanguage == language) continue;
					String translation = senseTranslation.getSimpleLemma().toLowerCase();

					Triple<String, String, Language> translationTriple =
						new Triple<String, String, Language>(
							senseLemma, translation, translationLanguage);
					if (translationsDone.contains(translationTriple)) continue;
					translationsDone.add(translationTriple);

					// valid translation: count it!
					IntegerCounter<String> translationCounter =
						traslationCounters.get(translationLanguage);
		    		if (translationCounter == null)
		    		{
		    			translationCounter = new IntegerCounter<String>();
		    			traslationCounters.put(translationLanguage, translationCounter);
		    		}
		    		translationCounter.count(translation);
				}
			}
		}

		Multimap<Language, ScoredItem<String>> babelTraslations =
				new HashMultimap<Language, ScoredItem<String>>();
		for (Language otherLanguage : traslationCounters.keySet())
		{
			IntegerCounter<String> translationCounter = traslationCounters.get(otherLanguage);
			for (String translation : translationCounter.keySet())
				babelTraslations.put(otherLanguage,
									 new ScoredItem<String>(translation,
															translationCounter.get(translation)));
		}
		return babelTraslations;
	}

	/**
	 * Get the glosses of a specific Babel synset, given a concept identifier
	 * (Babel synset ID).
	 *
	 * @param concept
	 *            a concept identifier (babel synset ID)
	 * @return the glosses of a specific {@link BabelSynset} to which the input
	 *         Babel synset ID corresponds
	 * @throws IOException
	 */
	public List<BabelGloss> getGlosses(String concept) throws IOException
	{
		TermQuery q =
			new TermQuery(new Term(BabelNetIndexField.ID.toString(), concept));
		List<BabelGloss> glossList = new ArrayList<BabelGloss>();

		// interroga l'indice e restituisce il documento delle glosse se esiste
		TopDocs docs = glosses.search(q, MAXIMUM_NUMBER_OF_SYNSETS);

		for (ScoreDoc scoreDoc : docs.scoreDocs)
		{
		    Document doc = glosses.doc(scoreDoc.doc);
		    String[] storedGlosses = doc.getValues(BabelNetIndexField.GLOSS.toString());

		    for (int i = 0; i < storedGlosses.length; i++)
		    {
		    	String storedGloss = storedGlosses[i];
		    	String[] split = storedGloss.split("\t");
		    	if (split.length != 4)
		    		throw new RuntimeException("Invalid gloss: " + storedGloss);

		    	Language language = Language.valueOf(split[0]);
		    	//retro compatibilità
		    	if(split[1].equals("WIKIWN")) split[1] = "WN";
		    	BabelSenseSource senseSource = BabelSenseSource.valueOf(split[1]);
		    	String sense = split[2];
		    	String gloss = split[3];

		    	BabelGloss bGloss =
		    		new BabelGloss(senseSource, sense, language, gloss);
		    	glossList.add(bGloss);
		    }
		}

		return glossList;
	}

	/**
	 * Creates a new instance of {@link BabelSynsetIterator}.
	 *
	 * @return an instance of a {@link BabelSynsetIterator}.
	 * @throws IOException
	 */
	public BabelSynsetIterator getSynsetIterator()
	{
		return new BabelSynsetIterator(dictionary);
	}

	/**
	 * Creates a new instance of {@link BabelOffsetIterator}.
	 *
	 * @return an instance of a {@link BabelOffsetIterator}.
	 * @throws IOException
	 */
	public BabelOffsetIterator getOffsetIterator()
	{
		return new BabelOffsetIterator(dictionary);
	}

	/**
	 * Creates a new instance of {@link BabelLexiconIterator}.
	 *
	 * @return an instance of a {@link BabelLexiconIterator}.
	 * @throws IOException
	 */
	public BabelLexiconIterator getLexiconIterator()
	{
		return new BabelLexiconIterator(lexicon);
	}
}
