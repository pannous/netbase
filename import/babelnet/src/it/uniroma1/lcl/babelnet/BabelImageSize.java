package it.uniroma1.lcl.babelnet;

/**
 * Size of the BabelImage thumb
 *
 * @author vannella 
 * 
 */
public enum BabelImageSize { 

	/**  
	 * Size 120px
	 */
	SMALL(120),
	
	/**
	 *  Size 360px
	 */	
	MEDIUM(360),
	
	/**
	 *  Size 640px
	 */
	BIG(640);
	
	/**
	 * Image size
	 */
	private int size;
	
	BabelImageSize(int size) { this.size = size; }
	public Integer getSize() { return size; }
}
