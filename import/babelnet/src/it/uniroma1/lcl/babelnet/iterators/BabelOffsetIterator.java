package it.uniroma1.lcl.babelnet.iterators;

import it.uniroma1.lcl.babelnet.BabelNet;
import it.uniroma1.lcl.babelnet.BabelNetIndexField;

import java.io.IOException;
import java.util.ConcurrentModificationException;

import org.apache.lucene.document.Document;
import org.apache.lucene.search.IndexSearcher;

/** 
 * Abstract iterator over {@link BabelNet}'s synset offset
 * 
 * @author ehrmann, vannella
 */
public class BabelOffsetIterator extends BabelIterator<String>
{
	public BabelOffsetIterator(IndexSearcher dictionary) { super(dictionary); }
	
	@Override
	public String next()
	{
		if(reader.isDeleted(currentIndex))
			throw new ConcurrentModificationException();
		
		try
		{
			Document doc = reader.document(currentIndex++);
			
			// if it'is the doc with version, skip this Lucene document 
			String version = doc.get(BabelNetIndexField.VERSION.toString());
		    if (version != null) return next();
		    
			return doc.get(BabelNetIndexField.ID.toString());
		}
		catch (IOException e)
		{
			throw new RuntimeException("Cannot return next: " + currentIndex);
		}
	}
}
