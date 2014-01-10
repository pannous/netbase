package it.uniroma1.lcl.knowledge.graph;

import it.uniroma1.lcl.jlt.util.Strings;
import edu.mit.jwi.item.IPointer;

/**
 * A single edge in a {@link KnowledgeGraphPath}.
 * 
 * @author ponzetto
 * 
 */
public class KnowledgeGraphPathEdge
{
	private final String src;
	
	private final String target;
	
	private final IPointer pointer;
	
	private final String pointerSymbol;
	
	private final double weight;
	
	public KnowledgeGraphPathEdge(String src, String target,
										String pointerSymbol, IPointer pointer,
										double weight)
	{
		this.src = src;
		this.target = target;
		this.pointer =  pointer;
		this.pointerSymbol =  pointerSymbol;
		this.weight = weight;
	}

	public String getSrc() { return src; }

	public String getTarget() { return target; }

	public IPointer getPointer() { return pointer; }
	
	public String getPointerSymbol() { return pointerSymbol; }
	
	public double getWeight() { return weight; }
	
	@Override
	public String toString()
	{
		return "("+src+","+pointerSymbol+":"+Strings.format(weight)+","+target+")";
	}
}
