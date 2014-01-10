package it.uniroma1.lcl.babelnet;

import edu.mit.jwi.item.IPointer;
import it.uniroma1.lcl.jlt.util.Language;
import it.uniroma1.lcl.jlt.util.Strings;

/**
 * Class to model an edge in the BabelNet graph.
 * 
 * @author ponzetto
 * 
 */
public class BabelNetGraphEdge
{
	private final Language language;
	
	private final IPointer pointer;

	private final String target;

	private double weight;

	private double normalizedWeight;

	public BabelNetGraphEdge(Language language, BabelPointer pointer, String target)
	{
		this(language, pointer, target, 0.0, 0.0);
	}
	
	public BabelNetGraphEdge(Language language, BabelPointer pointer, String target,
							  double weight, double normalizedWeight)
	{
		this.language = language;
		this.pointer = pointer;
		this.target = target;
		
		this.weight = weight;
		this.normalizedWeight = normalizedWeight;
	}
	
	public Language getLanguage() { return language; }

	public IPointer getPointer() { return pointer; }

	/**
	 * Getter for the weight of the relation
	 * 
	 * @return the weight of <b>this</b> edge.
	 */
	public double getWeight() { return weight; }

	public void setWeight(double weight) { this.weight = weight; }
	
	/**
	 * Getter for the weight normalized across all relations of the same
	 * knowledge source, namely such that \sum_{e \in E_out^{WN/WIKI}} w(e) = 1
	 * 
	 * @return the normalized weight of <b>this</b> edge.
	 */
	public double getNormalizedWeight() { return normalizedWeight; }

	public void setNormalizedWeight(double normalizedWeight)
	{ this.normalizedWeight = normalizedWeight; }
	
	public String getTarget() { return target; }
	
	/**
	 * Creates a new instance of {@link BabelNetGraphEdge} from an input record
	 * in the same format as found in {@link BabelNetIndexField#RELATION}
	 * 
	 * @param edge
	 * @return creates a {@link BabelNetGraphEdge} from a string representation.
	 */
	public static BabelNetGraphEdge fromString(String edge)
	{
    	// example EN_r_bn:00026887n_0.00001_0.00001
    	final String[] relationSplit = edge.split("_");
    	if (relationSplit.length != 5)
    		throw new RuntimeException("Invalid relation: " + edge);
    	
    	return new BabelNetGraphEdge(
    		Language.valueOf(relationSplit[0]),
    		BabelPointer.getPointerType(relationSplit[1]),
    		relationSplit[2],
    		Double.valueOf(relationSplit[3]),
    		Double.valueOf(relationSplit[4]));
	}
	
	@Override
	public String toString()
	{
		return
			new StringBuffer().
				append(language).append("_").
				append(pointer.getSymbol()).append("_").
				append(target).append("_").
				append(Strings.format(weight)).append("_").
				append(Strings.format(normalizedWeight)).toString();
	}

	@Override
	public int hashCode()
	{
		final int prime = 31;
		int result = 1;
		result = prime * result
				+ ((language == null) ? 0 : language.hashCode());
		long temp;
		temp = Double.doubleToLongBits(normalizedWeight);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		result = prime * result + ((pointer == null) ? 0 : pointer.hashCode());
		result = prime * result + ((target == null) ? 0 : target.hashCode());
		temp = Double.doubleToLongBits(weight);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		return result;
	}

	@Override
	public boolean equals(Object obj)
	{
		if (obj instanceof BabelNetGraphEdge)
		{
			BabelNetGraphEdge other = (BabelNetGraphEdge) obj;
			return 
				language == other.getLanguage() &&
				pointer.getSymbol().equals(other.getPointer().getSymbol()) &&
				target.equals(other.getTarget()) &&
				Double.doubleToLongBits(weight) != 
					Double.doubleToLongBits(other.getWeight()) &&
				Double.doubleToLongBits(normalizedWeight) != 
					Double.doubleToLongBits(other.getNormalizedWeight());
		}
		return false;
	}
}
