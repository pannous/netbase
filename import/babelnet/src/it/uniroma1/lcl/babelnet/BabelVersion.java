package it.uniroma1.lcl.babelnet;

/**
 * 
 * Version of BabelNet
 * 
 * @author navigli
 *
 */
public enum BabelVersion
{
	UNKNOWN("unknown"),
	PRE_2_0("< 2.0"),
	V2_0("2.0");
	
	private String ver;
	public String toString() { return ver; }
	
	private BabelVersion(String ver) { this.ver = ver; }
}
