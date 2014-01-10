package it.uniroma1.lcl.babelnet;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import java.io.Serializable;

import edu.mit.jwi.item.IPointer;

/**
 * Default, hard-coded, implementation of {@code IPointer} for {@link BabelNet}.
 * Models a semantic pointer in {@link BabelNet}. Includes an associative (ie
 * semantically unspecified) relation.
 *
 * @see IPointer
 */
public class BabelPointer implements IPointer, Serializable
{
	private static final long serialVersionUID = -8065288275131372908L;

	// Wikipedia relations
	public static final BabelPointer SEMANTICALLY_RELATED	= new BabelPointer("r", "Semantically related form", "related");

	// WordNet glosses relations
	public static final BabelPointer GLOSS_MONOSEMOUS		= new BabelPointer("gmono", "Gloss related form (monosemous)", "gloss-related");
	public static final BabelPointer GLOSS_DISAMBIGUATED	= new BabelPointer("gdis", "Gloss related form (disambiguated)", "gloss-related");

	// WordNet relations
	public static final BabelPointer ALSO_SEE 				= new BabelPointer("^", 	"Also See", "also-see");
	public static final BabelPointer ANTONYM 				= new BabelPointer("!", 	"Antonym", "antonym");
	public static final BabelPointer ATTRIBUTE 				= new BabelPointer("=", 	"Attribute", "attrib");
	public static final BabelPointer CAUSE 					= new BabelPointer(">", 	"Cause", "cause");
	public static final BabelPointer DERIVATIONALLY_RELATED	= new BabelPointer("+", 	"Derivationally related form", "deriv");
	public static final BabelPointer ENTAILMENT 			= new BabelPointer("*", 	"Entailment", "entails");
	public static final BabelPointer HYPERNYM 				= new BabelPointer("@", 	"Hypernym", "is-a");
	public static final BabelPointer HYPERNYM_INSTANCE 		= new BabelPointer("@i", "Instance hypernym", "is-a");
	public static final BabelPointer HYPONYM 				= new BabelPointer("~", 	"Hyponym", "is-a");
	public static final BabelPointer HYPONYM_INSTANCE 		= new BabelPointer("~i", "Instance hyponym", "is-a");
	public static final BabelPointer HOLONYM_MEMBER 		= new BabelPointer("#m", "Member holonym", "part-of");
	public static final BabelPointer HOLONYM_SUBSTANCE 		= new BabelPointer("#s", "Substance holonym", "part-of");
	public static final BabelPointer HOLONYM_PART 			= new BabelPointer("#p", "Part holonym", "part-of");
	public static final BabelPointer MERONYM_MEMBER 		= new BabelPointer("%m", "Member meronym", "part-of");
	public static final BabelPointer MERONYM_SUBSTANCE 		= new BabelPointer("%s", "Substance meronym", "part-of");
	public static final BabelPointer MERONYM_PART 			= new BabelPointer("%p", "Part meronym", "part-of");
	public static final BabelPointer PARTICIPLE 			= new BabelPointer("<", 	"Participle", "participle");
	public static final BabelPointer PERTAINYM 				= new BabelPointer("\\", "Pertainym (pertains to nouns)", "pertains-to");
	public static final BabelPointer REGION 				= new BabelPointer(";r", "Domain of synset - REGION", "domain");
	public static final BabelPointer REGION_MEMBER 			= new BabelPointer("-r", "Member of this domain - REGION", "domain");
	public static final BabelPointer SIMILAR_TO 			= new BabelPointer("&", 	"Similar To", "sim");
	public static final BabelPointer TOPIC 					= new BabelPointer(";c", "Domain of synset - TOPIC", "topic");
	public static final BabelPointer TOPIC_MEMBER 			= new BabelPointer("-c", "Member of this domain - TOPIC", "topic");
	public static final BabelPointer USAGE 					= new BabelPointer(";u", "Domain of synset - USAGE", "usage");
	public static final BabelPointer USAGE_MEMBER 			= new BabelPointer("-u", "Member of this domain - USAGE", "usage");
	public static final BabelPointer VERB_GROUP 			= new BabelPointer("$", 	"Verb Group", "verb_group");

	private final String fSymbol;
    private final String fName;
    private final String shortName;

    /**
     * Private constructor because this is like an enum type
     */
    public BabelPointer(String symbol, String name, String shortName)
    {
    	this.fSymbol = symbol;
        this.fName = name;
        this.shortName = shortName;
    }

//    @Override
    public String getSymbol() { return fSymbol; }

//    @Override
    public String getName() { return fName; }

    public String getShortName() { return shortName; }

    @Override
    public String toString()
    {
        return fName.toLowerCase().replace(' ', '_').replace(",", "");
    }

    @Override
    public boolean equals(Object obj)
    {
    	return
    		fName.equals(((BabelPointer) obj).fName)
    		&&
    	  fSymbol.equals(((BabelPointer) obj).fSymbol);
    }

    @Override
    public int hashCode()
    {
    	return fName.hashCode()+fSymbol.hashCode();
    }

    /**
     * Static map to speed up access
     */
    private static final Map<String, BabelPointer> pointerMap;

    /**
     * All the pointers
     */
    private static final Set<BabelPointer> pointerSet;

    static
    {
		// get the instance fields
		Field[] fields = BabelPointer.class.getFields();
		List<Field> instanceFields = new ArrayList<Field>();
		for (Field field : fields)
			if (field.getGenericType() == BabelPointer.class)
				instanceFields.add(field);

		// the backing collections
		Set<BabelPointer> hiddenSet =
			new LinkedHashSet<BabelPointer>(instanceFields.size());
		Map<String, BabelPointer> hiddenMap =
			new LinkedHashMap<String, BabelPointer>(instanceFields.size()-1);

		BabelPointer ptr;
		for(Field field : instanceFields)
		{
			try
			{
				ptr = (BabelPointer)field.get(null);
				if (ptr == null) continue;
				hiddenSet.add(ptr);
				hiddenMap.put(ptr.getSymbol(), ptr);
			}
			catch(IllegalAccessException e)
			{
				// safe to ignore here...
			}
		}

		// make the collections unmodifiable
		pointerSet = Collections.unmodifiableSet(hiddenSet);
		pointerMap = Collections.unmodifiableMap(hiddenMap);
    }

	/**
	 * Emulates the Enum.values() function. Returns an unmodifiable collection
	 * of all the pointers declared in this class, in the order they are
	 * declared.
	 */
	public static Collection<BabelPointer> values()
	{
		return pointerSet;
	}

    /**
	 * Returns the pointer type (static final instance) that matches the
	 * specified pointer symbol.
	 *
	 * @throws IllegalArgumentException
	 *             if the symbol does not correspond to a known pointer.
	 */
    public static BabelPointer getPointerType(String symbol)
    {
        BabelPointer pointerType = pointerMap.get(symbol);
        return pointerType;
    }
}
