package it.uniroma1.lcl.knowledge;

import it.uniroma1.lcl.jlt.util.Collections;
import it.uniroma1.lcl.jlt.util.DirectoryFileManager;
import it.uniroma1.lcl.jlt.util.Files;
import it.uniroma1.lcl.jlt.util.Strings;
import it.uniroma1.lcl.jlt.util.Timer;
import it.uniroma1.lcl.jlt.util.Triple;
import it.uniroma1.lcl.knowledge.KnowledgeBasePathIndex.KnowledgeBasePathIndexField;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.store.SimpleFSDirectory;
import org.apache.lucene.util.Version;

/**
 * A factory to create a {@link KnowledgeBasePathIndex}.
 * 
 * @author ponzetto
 *
 */
public class KnowledgeBasePathIndexFactory
{
	private static final Log log = LogFactory.getLog(KnowledgeBasePathIndexFactory.class);
	
	private static final String TXT_FILE_DIR = "/txt/";

	private static final String INDEX_DIR = "/index/";
	
	private static final String PATH_SEPARATOR = "|";
	
	private static final int NSPLITS = 10;
	
	/**
	 * Creates the files containing all paths for all concepts up to a certain
	 * depth
	 * 
	 * @param kb
	 * @throws IOException
	 */
	public static void createIndexFiles(KnowledgeBase kb) throws IOException
	{
		createIndexFiles(kb, new HashSet<String>());
	}

	/**
	 * Creates the files containing all paths for all concepts up to a certain
	 * depth
	 * 
	 * @param kb
	 * @param concepts
	 * @throws IOException
	 */
	public static void createIndexFiles(KnowledgeBase kb, Collection<String> concepts) throws IOException
	{
		Iterator<String> conceptIterator = kb.getConceptIterator();
		String outputDir = 
			KnowledgeConfiguration.getInstance().getKnowledgeBasePathIndexDir(kb)+TXT_FILE_DIR;
		if (!Files.isDirectoryEmpty(new File(outputDir)))
		{
			log.warn(outputDir + " IS NOT EMPTY, BAILING OUT ...");
			return;
		}
		
		int maxSearchDepth = getMaxSearchDepth();
		DirectoryFileManager dirManager = 
			new DirectoryFileManager(outputDir, 1000);
		log.info("CREATING THE INDEXABLE PATH FILES FOR: " + 
				 kb.name() + " INTO " + outputDir);

		int idx = 0;
		List<String> goodConcepts = new ArrayList<String>();
		while (conceptIterator.hasNext())
		{
			String concept = conceptIterator.next();
			if (concepts.isEmpty() || concepts.contains(concept))
				goodConcepts.add(concept);

			idx++;
			if ((idx % 100000) == 0) log.info("ITERATED THROUGH " + idx + " CONCEPTS SO FAR ...");
				
		}

		List<List<String>> splitConcepts = Collections.split(goodConcepts,
															 goodConcepts.size()/NSPLITS);
		int nThreads = splitConcepts.size();
		ExecutorService threadExecutor = Executors.newFixedThreadPool(nThreads);
		CountDownLatch doneSignal = new CountDownLatch(nThreads);
		
		for (int i = 0; i < nThreads; i++)
		{
			ConceptIndexer worker =
				new ConceptIndexer(splitConcepts.get(i), i, dirManager,
								   kb, maxSearchDepth, doneSignal);
			threadExecutor.execute(worker);		
		}
		
		Timer timer = new Timer();
		try
	    {
	    	doneSignal.await();
	    	threadExecutor.shutdown();
	    }
	    catch (InterruptedException ie)
	    {
	    	ie.printStackTrace();
	    }
	    timer.tick("TO CREATE INDEXABLE FILES");
	}

	/**
	 * Creates the {@link Document} for {@link KnowledgeBasePathIndex}
	 * 
	 * @param topDir the directory where txt and index files are found
	 * @throws IOException
	 */
	public static void createIndex(String topDir) throws IOException
	{
		File txtFileDir = new File(topDir+TXT_FILE_DIR);
		File indexDir = new File(topDir+INDEX_DIR);
		createIndex(txtFileDir, indexDir);
	}
	
	/**
	 * Creates the {@link Document} for {@link KnowledgeBasePathIndex}
	 * 
	 * @param kb the knowledge base to index
	 * @throws IOException
	 */
	public static void createIndex(KnowledgeBase kb) throws IOException
	{
		File txtFileDir = 
			new File(
				KnowledgeConfiguration.getInstance().getKnowledgeBasePathIndexDir(kb)+TXT_FILE_DIR);
		File indexDir = 
			new File(
				KnowledgeConfiguration.getInstance().getKnowledgeBasePathIndexDir(kb)+INDEX_DIR);
		createIndex(txtFileDir, indexDir);
	}	
	
	/**
	 * Creates the {@link Document} for {@link KnowledgeBasePathIndex}
	 * 
	 * @param txtFileDir where the path txt files are
	 * @param indexDir the directory where the index is created
	 * 
	 * @throws IOException
	 */
	public static void createIndex(File txtFileDir, File indexDir) throws IOException
	{
		if (!indexDir.exists()) indexDir.mkdirs();
		else if (!Files.isDirectoryEmpty(indexDir))
		{
			log.warn(indexDir + " IS NOT EMPTY, BAILING OUT ...");
			return;
		}
		
		IndexWriter indexWriter = new IndexWriter(
				new SimpleFSDirectory(indexDir),
				new StandardAnalyzer(Version.LUCENE_29),
				true,
				IndexWriter.MaxFieldLength.UNLIMITED);
		
		int counter = 0;
		FilenameFilter filter = new FilenameFilter()
		{
			public boolean accept(File dir, String name)
			{ return name.endsWith(".txt"); }
		};
		Collection<File> files = Files.listFiles(txtFileDir, filter, true);
		
		for (File file : files)
		{
			BufferedReader reader = new BufferedReader(new FileReader(file));

			Set<List<String>> paths = new HashSet<List<String>>();
			while (reader.ready())
			{
				String line = reader.readLine();
				paths.add(Arrays.asList(line.split(" ")));
			}
			reader.close();
			
			List<Document> docs = getRecords(paths);
			for (Document doc : docs) indexWriter.addDocument(doc);
			
			if ((counter%1000)==0)
				log.info("\tINDEXED PATHS FOR " +  counter + " CONCEPTS SO FAR ... ");
			counter++;
		}
		log.info("\tINDEXED " + counter + " CONCEPTS, DONE!");
		
		// closes the index writers
		indexWriter.optimize(); indexWriter.close();
	}
	
	/**
	 * Creates the {@link Document} for {@link KnowledgeBasePathIndex} for
	 * a bunch of input concepts only
	 * 
	 * @param kb
	 * @throws IOException
	 */
	public static void createIndex(KnowledgeBase kb, Collection<String> concepts) throws IOException
	{
		File txtFileDir = 
			new File(
				KnowledgeConfiguration.getInstance().getKnowledgeBasePathIndexDir(kb)+TXT_FILE_DIR);
		// init the index dirs and the index writer
		File indexDir = 
			new File(
				KnowledgeConfiguration.getInstance().getKnowledgeBasePathIndexDir(kb)+INDEX_DIR);

		if (!indexDir.exists()) indexDir.mkdirs();
		else if (!Files.isDirectoryEmpty(indexDir))
		{
			log.warn(indexDir + " IS NOT EMPTY, BAILING OUT ...");
			return;
		}
		
		IndexWriter indexWriter = new IndexWriter(
				new SimpleFSDirectory(indexDir),
				new StandardAnalyzer(Version.LUCENE_29),
				true,
				IndexWriter.MaxFieldLength.UNLIMITED);
		
		int counter = 0;
		FilenameFilter filter = new FilenameFilter()
		{
			public boolean accept(File dir, String name)
			{ return name.endsWith(".txt"); }
		};
		Collection<File> files = Files.listFiles(txtFileDir, filter, true);
		
		// the intermediate concepts @depth 2 from the starting nodes
		Set<String> intermediate = new HashSet<String>();
		
		// PHASE 1. index all paths starting with concepts of interest
		for (File file : files)
		{
			String fileName = file.getName();
			String concept = fileName.substring(0, fileName.indexOf("."));
			if (!concepts.contains(concept)) continue;
			
			BufferedReader reader = new BufferedReader(new FileReader(file));

			Set<List<String>> paths = new HashSet<List<String>>();
			while (reader.ready())
			{
				String line = reader.readLine();
				String[] path = line.split(" ");
				paths.add(Arrays.asList(path));
				intermediate.add(path[path.length-1]);
			}
			reader.close();
			
			List<Document> docs = getRecords(paths);
			for (Document doc : docs) indexWriter.addDocument(doc);
			
			if ((counter%1000)==0)
				log.info("\tINDEXED STARTING PATHS FOR " +  counter + " CONCEPTS SO FAR ... ");
			counter++;
		}
		log.info("\tINDEXED " + counter + " STARTING CONCEPTS, DONE!");
		
		log.info("NOW INDEXING ENDING PATHS FOR " + intermediate.size() +
				 " INTERMEDIATE CONCEPTS");
		
		// PHASE 2. index all paths ending with concepts of interest
		counter = 0;
		for (File file : files)
		{
			String fileName = file.getName();
			String concept = fileName.substring(0, fileName.indexOf("."));
			if (concepts.contains(concept)) continue; // already seen
			if (!intermediate.contains(concept)) continue; // interested only
															// in intermediate
			
			BufferedReader reader = new BufferedReader(new FileReader(file));

			Set<List<String>> paths = new HashSet<List<String>>();
			while (reader.ready())
			{
				String line = reader.readLine();
				String[] path = line.split(" ");
				if (concepts.contains(path[path.length-1]))
					paths.add(Arrays.asList(path));
			}
			reader.close();
			
			List<Document> docs = getRecords(paths);
			for (Document doc : docs) indexWriter.addDocument(doc);
			
			if ((counter%1000)==0)
				log.info("\tINDEXED ENDING PATHS FOR " +  counter + " CONCEPTS SO FAR ... ");
			counter++;
		}
		log.info("\tINDEXED " + counter + " ENDING CONCEPTS, DONE!");
		
		// closes the index writers
		indexWriter.optimize(); indexWriter.close();
	}
	
	/**
	 * Creates a list of {@link Document}s, each containing the paths between
	 * a start and an end. Example: given paths
	 * 
	 * 00000001 ~ 0.07692 00000002
	 * 00000001 ~ 0.07692 00000002 ~ 1 00000003
	 * 00000001 ! 1 00000004
	 * 00000001 ! 1 00000004 ~ 0.00533 00000003
	 * 00000001 %p 1 00000005 ~ 0.00533 00000002
	 * 
	 * FORMAT: src (relation weight target)+
	 * 
	 * creates a bunch of docs with indexed start/end fields
	 * 
	 * 00000001/00000002
	 * 00000001/00000003
	 * 00000001/00000004
	 * 
	 * and intermediate node paths as last field, e.g. for record where
	 * start=00000001 and end=00000003
	 * 
	 * ~ 0.07692 00000002 ~ 1|! 1 00000004 ~ 0.00533 
	 * 
	 * @param concept
	 * @param paths
	 * @return
	 * 
	 */
	private static List<Document> getRecords(Set<List<String>> paths)
	{
		// the Lucene records
		List<Document> docs = new ArrayList<Document>();
		
		// stores intermediate nodes for each star/end path
		Map<Triple<String, String, Integer>, StringBuffer> pathsBuffers =
			new HashMap<Triple<String,String, Integer>, StringBuffer>();
		
		for (List<String> path : paths)
		{
			String firstPathElement = path.get(0);
			String lastPathElement = path.get(path.size()-1);
			
			if (path.size() > 1)
			{
				int depth = (path.size()-1)/3;
				Triple<String, String, Integer> startEndDepth =
					new Triple<String, String, Integer>(firstPathElement,
														lastPathElement, depth);

				StringBuffer pathsBuffer = pathsBuffers.get(startEndDepth);
				if (pathsBuffer == null)
				{
					pathsBuffer = new StringBuffer();
					pathsBuffers.put(startEndDepth, pathsBuffer);
				}
				
				// set the remaining path in PATHS
				for (int elem = 1; elem < path.size()-1; elem++)
				{
					String element = path.get(elem);
					pathsBuffer.append(element).append(" ");
				}
				pathsBuffer.deleteCharAt(pathsBuffer.length()-1);
				pathsBuffer.append(PATH_SEPARATOR);
			}
		}
		
		// for each start/end/depth combination
		for (Triple<String, String, Integer> startEndDepth : pathsBuffers.keySet())
		{
			String firstPathElement = startEndDepth.getFirst();
			String lastPathElement = startEndDepth.getSecond();
			Integer depth = startEndDepth.getThird();
			StringBuffer pathsBuffer = pathsBuffers.get(startEndDepth);

			if (pathsBuffer.length() > 0)
				pathsBuffer.deleteCharAt(pathsBuffer.length()-1);
			String pathsString = pathsBuffer.toString();
			
			// log.info("PATH DOC FOR " + firstPathElement + 
			//		"\n\t\t START: " + firstPathElement +
			//		"\n\t\t START: " + lastPathElement +
			//		"\n\t\t PATHS: " + pathsString);
			
			// creates a new document
			Document doc = new Document();
			doc.add(new Field(KnowledgeBasePathIndexField.START.toString(),
					firstPathElement, Field.Store.YES, Field.Index.NOT_ANALYZED));
			doc.add(new Field(KnowledgeBasePathIndexField.END.toString(),
					lastPathElement, Field.Store.YES, Field.Index.NOT_ANALYZED));
			doc.add(new Field(KnowledgeBasePathIndexField.DEPTH.toString(), 
					depth.toString(), Field.Store.NO, Field.Index.NOT_ANALYZED));
			doc.add(new Field(KnowledgeBasePathIndexField.PATHS.toString(), 
					pathsString, Field.Store.YES, Field.Index.NOT_ANALYZED));
			
			docs.add(doc);
		}
		return docs;
	}

	protected static int getMaxSearchDepth()
	{
		// default is to search at maximum depth 2
		return 2;
	}
	
	private static class ConceptIndexer implements Runnable
	{
		private final List<String> concepts;
		
		private final int maxSearchDepth;
	
		private final int id;
		
		private final KnowledgeBase kb;
		
		private final CountDownLatch doneSignal;
		
		private final DirectoryFileManager dirManager;
		
		public ConceptIndexer(List<String> concepts, int id,
							   DirectoryFileManager dirManager,
							   KnowledgeBase kb, int maxSearchDepth,
							   CountDownLatch doneSignal)
		{
			this.id = id;
			this.kb = kb;
			this.concepts = concepts;
			this.dirManager = dirManager;
			this.doneSignal = doneSignal;
			this.maxSearchDepth = maxSearchDepth;
		}

		@Override
		public void run()
		{
			try
			{
				getConcepts();
			}
			catch (Exception e)
			{
				log.warn("EXCEPTION: " + e + " STACKTRACE FOLLOWS:");
				e.printStackTrace();
			}
			finally
			{
				doneSignal.countDown();
			}
			
		}
		
		public void getConcepts() throws IOException
		{
			int counter = 0;
			Timer timer = new Timer();
			for (String concept : concepts)
			{
				Set<List<String>> paths = kb.getAllPathsFrom(concept, maxSearchDepth);

				FileWriter writerFrom = dirManager.getFileWriter(concept+".txt");
				
				// saves the paths starting from the concept
				for (List<String> path : paths)
					writerFrom.write(Strings.join(path)+"\n");
				
				writerFrom.close();
				
				if ((counter%100)==0)
					timer.tick("\t[WORKER] " + id +
							   " TO CREATE INDEXABLE FILES FOR " + 
							   counter + " CONCEPTS SO FAR ... ");
				counter++;
			}
		}
	}
	
	/**
	 * Use to create the index. Usage:
	 * 
	 *   java KnowledgeBasePathIndex <top-dir>
	 * 
	 * where <top-dir> is the parent directory of that containing the paths
	 * in txt format. Example: given the txt paths stored into a directory named
	 * as foo/bar/wordnet_paths/txt, simply run as
	 * 
	 *   java KnowledgeBasePathIndex foo/bar/wordnet_paths
	 * 
	 * @param args
	 */
	public static void main(String[] args)
	{
		try
		{
			if (args.length > 0)
			{
				KnowledgeBasePathIndexFactory.createIndex(args[0]);
			}
			else
			{
				System.err.println("USAGE: java KnowledgeBasePathIndex path-index dir");
				System.exit(0);
			}

			// FOR IN-HOUSE USE ONLY
			//
			// KnowledgeBase kb = KnowledgeBase.WORDNET;
			// KnowledgeBasePathIndexFactory.createIndexFiles(kb);
			// KnowledgeBasePathIndexFactory.createIndex(kb);
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
}
