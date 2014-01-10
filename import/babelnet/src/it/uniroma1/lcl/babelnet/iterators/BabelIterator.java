package it.uniroma1.lcl.babelnet.iterators;


import it.uniroma1.lcl.babelnet.BabelNet;

import java.io.IOException;
import java.util.Iterator;

import org.apache.lucene.index.IndexReader;
import org.apache.lucene.search.IndexSearcher;
import java.util.ConcurrentModificationException;
import org.apache.lucene.document.Document;

/**
 * Abstract iterator over {@link BabelNet}'s content
 * 
 * @param <T>
 * 
 * @author ehrmann, vannella
 */


public abstract class BabelIterator<T> implements Iterator<T> 	{

	protected int currentIndex;
	protected final IndexReader reader;
			
	  protected BabelIterator(IndexSearcher searcher) {
		    this.reader = searcher.getIndexReader();
		    this.currentIndex = 0;
		  
		    while(currentIndex < reader.maxDoc() && reader.isDeleted(currentIndex)) {
		      currentIndex++;
		    }
	  }
	
	  @Override
	  public boolean hasNext() {
		  return currentIndex < reader.maxDoc();
	  }
	
	  @Override
	  public void remove() {
		  throw new RuntimeException("Unsupported operation 'remove'");
	  }
	                
	  protected Document nextDoc() throws IOException {
		    if(reader.isDeleted(currentIndex)) {
		      throw new ConcurrentModificationException();
		    }
		    Document doc = reader.document(currentIndex++);
		                    
		    while(currentIndex < reader.maxDoc() && reader.isDeleted(currentIndex)) {
		      currentIndex++;
		    }
		                    
		    return doc;
	  }
}


