package it.uniroma1.lcl.knowledge;

import it.uniroma1.lcl.jlt.util.Timer;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.lucene.document.Document;
import org.apache.lucene.index.Term;
import org.apache.lucene.search.BooleanClause;
import org.apache.lucene.search.BooleanQuery;
import org.apache.lucene.search.Hits;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.TermQuery;
import org.apache.lucene.search.BooleanClause.Occur;

/**
 * A class to extract and store paths within a Lucene index.
 * 
 * @author ponzetto
 * 
 */
@SuppressWarnings("deprecation") // avoids warning from the Lucene crappola
public class KnowledgeBasePathIndex
{
	private static final Log log = LogFactory.getLog(KnowledgeBasePathIndex.class);
	
	private static final String QUOTED_PATH_SEPARATOR = "\\|";
	
	public static enum KnowledgeBasePathIndexField
	{
		// depth
		DEPTH,
		
		// the first node in a path
		START,

		// the last node in a path
		END,
		
		// the remainder of a path
		PATHS;
	}
	
	/**
	 * The {@link IndexSearcher} 
	 */
	private  IndexSearcher index;
	
	public KnowledgeBasePathIndex(KnowledgeBase kb)
	{
		String indexDir = null;
		
		try
		{ 
			indexDir =
				KnowledgeConfiguration.getInstance().getKnowledgeBasePathIndexDir(kb)+"/index";
			log.info("LOADING KNOWLEDGE BASE INDEX FROM: " + indexDir);
			index = new IndexSearcher(indexDir, true);
		}
		catch (IOException e)
		{ 
			throw new RuntimeException("Cannot find index: " + indexDir);
		}
	}

	public void close() throws IOException
	{
		index.close();
	}
	
	/**
	 * Method to collect all paths originating from the input concept.
	 * 
	 * @param srcConcept
	 * @return all paths starting from the input concept
	 * @throws IOException
	 */
	public Set<List<String>> getPathsFrom(String srcConcept) throws IOException
	{
		return getPathsFrom(srcConcept, null);
	}
	
	/**
	 * Method to collect all paths originating from the input concept
	 * with a certain depth.
	 * 
	 * @param srcConcept
	 * @param depth
	 * @return all paths starting from the input concept  up to a certain depth
	 * @throws IOException
	 */
	public Set<List<String>> getPathsFrom(String srcConcept, Integer depth) throws IOException
	{
		Query query = null;
		
	    if (depth != null)
	    {
	    	BooleanQuery bq = new BooleanQuery();
			bq.add(new BooleanClause(
						new TermQuery(
							new Term(KnowledgeBasePathIndexField.START.toString(),
									 srcConcept)), 
							Occur.MUST));
			bq.add(new BooleanClause(
						new TermQuery(
							new Term(KnowledgeBasePathIndexField.DEPTH.toString(),
									 depth.toString())), 
							Occur.MUST));
			query = bq;
	    }
	    else
	    {
		    query =
		    	new TermQuery(new Term(KnowledgeBasePathIndexField.START.toString(),
		    						   srcConcept));
	    }
		
	    Hits hits = index.search(query);
		return getPaths(hits);
	}

	/**
	 * Method to collect all paths between two input concept.
	 * 
	 * @param targetConcept
	 * @return all paths between input concepts
	 * @throws IOException
	 */
	public Set<List<String>> getPathsBetween(String sourceConcept,
											  String targetConcept) throws IOException
	{
		return getPathsBetween(sourceConcept, targetConcept, null);
	}
	
	
	/**
	 * Method to collect all paths between two input concept with a certain depth.
	 * 
	 * @param targetConcept
	 * @return all paths between input concepts up to a certain depth
	 * @throws IOException
	 */
	public Set<List<String>> getPathsBetween(String sourceConcept,
											 String targetConcept,
											 Integer depth) throws IOException
	{
		BooleanQuery q = new BooleanQuery();
		q.add(new BooleanClause(
				new TermQuery(
						new Term(KnowledgeBasePathIndexField.START.toString(),
								 sourceConcept)), 
						Occur.MUST));
		q.add(new BooleanClause(
				new TermQuery(
						new Term(KnowledgeBasePathIndexField.END.toString(),
								 targetConcept)), 
						Occur.MUST));
		if (depth != null)
			q.add(new BooleanClause(
					new TermQuery(
						new Term(KnowledgeBasePathIndexField.DEPTH.toString(),
								 depth.toString())), 
						Occur.MUST));
		
	    Hits hits = index.search(q);
		return getPaths(hits);
	}

	/**
	 * Method to collect all paths ending in the input concept.
	 * 
	 * @param targetConcept
	 * @return all paths ending in the input concept
	 * @throws IOException
	 */
	public Set<List<String>> getPathsTo(String targetConcept) throws IOException
	{
		return getPathsTo(targetConcept, null);
	}
	
	/**
	 * Method to collect all paths ending in the input concept with a certain depth.
	 * 
	 * @param targetConcept
	 * @return all paths ending in the input concept up to a certain depth
	 * @throws IOException
	 */
	public Set<List<String>> getPathsTo(String targetConcept, Integer depth) throws IOException
	{
		Query query = null;
		
	    if (depth != null)
	    {
	    	BooleanQuery bq = new BooleanQuery();
			bq.add(new BooleanClause(
						new TermQuery(
							new Term(KnowledgeBasePathIndexField.END.toString(),
									 targetConcept)), 
							Occur.MUST));
			bq.add(new BooleanClause(
						new TermQuery(
							new Term(KnowledgeBasePathIndexField.DEPTH.toString(),
									 depth.toString())), 
							Occur.MUST));
			query = bq;
	    }
	    else
	    {
		    query =
		    	new TermQuery(new Term(KnowledgeBasePathIndexField.END.toString(),
		    				  targetConcept));
	    }
		
	    Hits hits = index.search(query);
		return getPaths(hits);
	}
	
	private Set<List<String>> getPaths(Hits hits) throws IOException
	{
		Set<List<String>> paths = new HashSet<List<String>>();
		
		for (int i = 0; i < hits.length(); i++)
		{
	    	Document doc = hits.doc(i);
		    String srcConcept = 
		    	doc.get(KnowledgeBasePathIndexField.START.toString());
		    String targetConcept = 
		    	doc.get(KnowledgeBasePathIndexField.END.toString());
		    String pathsString = 
		    	doc.get(KnowledgeBasePathIndexField.PATHS.toString());

		    if (!pathsString.isEmpty())
		    {
			    for (String pathString : pathsString.split(QUOTED_PATH_SEPARATOR))
			    {
			    	List<String> path = new ArrayList<String>();

			    	path.add(srcConcept);
			    	for (String pathElement : pathString.split(" "))
			    		path.add(pathElement);
			    	path.add(targetConcept);
			    	
			    	// sanity check
			    	if ((path.size() % 3) != 1) log.warn("INVALID PATH: " + path);
			    	else paths.add(path);
			    }
		    }
		}
	    return paths;
	}
	
	/**
	 * Just for testing.
	 * 
	 * @param args contains (1) offset (2) knowledge base
	 */
	public static void main(String[] args)
	{
		// the defaults 
		String query = "02958343n"; // the first sense of car
		KnowledgeBase kb = KnowledgeBase.WORDNET; // PWN
		
		if (args.length > 1)
		{
			query = args[0];
			kb = KnowledgeBase.valueOf(args[1]);
		}
		
		try
		{
			KnowledgeBasePathIndex index = new KnowledgeBasePathIndex(kb);
			log.info("QUERYING " + query + " FROM KNOWLEDGE BASE: " + kb); 

			Timer timer = new Timer();
			StringBuffer buffer = new StringBuffer();

			timer.tick();
			Set<List<String>> fromPaths = index.getPathsFrom(query);
			timer.tick("TO GET PATHS FROM");

			buffer.append("LIST OF PATHS FROM:\n");
			for (List<String> fromPath : fromPaths)
			{
				buffer.append("\t").append(fromPath).append("\n");
			}
			buffer.append("\n");
			
			timer.tick();
			Set<List<String>> toPaths = index.getPathsTo(query, 1);
			timer.tick("TO GET PATHS TO");
			
			buffer.append("LIST OF PATHS TO:\n");
			for (List<String> toPath : toPaths)
			{
				buffer.append("\t").append(toPath).append("\n");
			}
			buffer.append("\n");

			log.info("PATH FOLLOWS:\n"+buffer.toString());
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
}
