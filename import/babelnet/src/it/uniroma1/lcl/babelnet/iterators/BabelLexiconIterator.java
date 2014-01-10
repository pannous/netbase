package it.uniroma1.lcl.babelnet.iterators;

import it.uniroma1.lcl.babelnet.BabelNet;
import it.uniroma1.lcl.babelnet.BabelNetIndexField;
import it.uniroma1.lcl.jlt.ling.Word;
import it.uniroma1.lcl.jlt.util.Language;

import java.io.IOException;
import java.util.ConcurrentModificationException;

import org.apache.lucene.document.Document;
import org.apache.lucene.search.IndexSearcher;

/** 
 * Abstract iterator over {@link BabelNet}'s lexicon
 * 
 * @author ehrmann, vannella
 */
public class BabelLexiconIterator extends BabelIterator<Word>
{
	public BabelLexiconIterator(IndexSearcher lexicon) { super(lexicon); }
	
	@Override
	public Word next()
	{
		if(reader.isDeleted(currentIndex))
			throw new ConcurrentModificationException();
		
		try
		{
			Document doc = reader.document(currentIndex++);
			
			// if it'is the doc with version, skip this Lucene document 
			String version = doc.get(BabelNetIndexField.VERSION.toString());
		    if (version != null) return next();
		    
			String pos = doc.get(BabelNetIndexField.POS.toString()); 
			String lemma = doc.get(BabelNetIndexField.LEMMA.toString());
			Language language =
				Language.valueOf(
					doc.get(BabelNetIndexField.LEMMA_LANGUAGE.toString()));
		
			return new Word(lemma, pos, "", language);
		}
		catch (IOException e)
		{
			throw new RuntimeException("Cannot return next: " + currentIndex);
		}
	}
}