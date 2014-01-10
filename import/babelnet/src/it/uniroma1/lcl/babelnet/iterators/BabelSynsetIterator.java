package it.uniroma1.lcl.babelnet.iterators;

import it.uniroma1.lcl.babelnet.BabelNet;
import it.uniroma1.lcl.babelnet.BabelNetIndexField;
import it.uniroma1.lcl.babelnet.BabelSynset;

import java.io.IOException;
import java.util.ConcurrentModificationException;

import org.apache.lucene.document.Document;
import org.apache.lucene.search.IndexSearcher;

/** 
 * Iterator over {@link BabelNet}'s synsets
 *  
 * @author ehrmann, vannella
 */
public class BabelSynsetIterator extends BabelIterator<BabelSynset>
{
	public BabelSynsetIterator(IndexSearcher dictionary) { super(dictionary); }
	 
	@Override
	public BabelSynset next()
	{
		if(reader.isDeleted(currentIndex))
			throw new ConcurrentModificationException();
		
		try
		{
			Document doc = reader.document(currentIndex++);

			// if it'is the doc with version, skip this Lucene document 
			String version = doc.get(BabelNetIndexField.VERSION.toString());
		    if (version != null) return next();

		    return BabelNet.getBabelSynsetFromDocument(doc);
		}
		catch (IOException e)
		{
			throw new RuntimeException("Cannot return next: " + currentIndex);
		}
	}
}
