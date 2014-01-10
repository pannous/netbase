package it.uniroma1.lcl.babelnet;

import it.uniroma1.lcl.babelnet.iterators.*;
import it.uniroma1.lcl.jlt.util.Language;
import it.uniroma1.lcl.jlt.util.ScoredItem;

import java.io.IOException;
import java.util.*;

import com.google.common.collect.Multimap;

import edu.mit.jwi.item.IPointer;
import edu.mit.jwi.item.POS;

/**
 * A demo class to test {@link BabelNet}'s various features.
 *
 * @author ponzetto
 */
public class BabelNetDemo {
    /**
     * A demo to see the senses of a word.
     *
     * @param lemma
     * @param languageToSearch
     * @param includeRedirections
     * @param languagesToPrint
     * @throws IOException
     */
    public static void testDictionary(String lemma, Language languageToSearch,
                                      boolean includeRedirections,
                                      Language... languagesToPrint) throws IOException {
        BabelNet bn = BabelNet.getInstance();
        System.out.println("SENSES FOR \"" + lemma + "\"");
        List<BabelSense> senses =
                bn.getSenses(languageToSearch, lemma, POS.NOUN, includeRedirections);
        Collections.sort(senses, new BabelSenseComparator());
        for (BabelSense sense : senses)
            System.out.println("\t=>" + sense.getSenseString());
        System.out.println();
        System.out.println("SYNSETS WITH \"" + lemma + "\"");
        List<BabelSynset> synsets =
                bn.getSynsets(languageToSearch, lemma, POS.NOUN, includeRedirections);
        Collections.sort(synsets, new BabelSynsetComparator(lemma));
        for (BabelSynset synset : synsets)
            System.out.println(
                    "\t=>(" + synset.getId() +
                            ") SOURCE: " + synset.getSynsetSource() +
                            ") TYPE: " + synset.getSynsetType() +
                            "; WN SYNSET: " + synset.getWordNetOffsets() +
                            "; MAIN SENSE: " + synset.getMainSense() +
                            "; SENSES: " + synset.toString(languagesToPrint));
        System.out.println();
    }

    /**
     * A demo to see the senses of a word.
     *
     * @param lemma
     * @param languageToSearch
     * @param includeRedirections
     * @throws IOException
     */
    public static void testDictionary(String lemma, Language languageToSearch,
                                      boolean includeRedirections,
                                      BabelSenseSource... allowedSources) throws IOException {
        BabelNet bn = BabelNet.getInstance();
        System.out.println("SENSES FOR \"" + lemma + "\"");
        List<BabelSense> senses =
                bn.getSenses(languageToSearch, lemma, POS.NOUN, includeRedirections,
                        allowedSources);
        Collections.sort(senses, new BabelSenseComparator());
        for (BabelSense sense : senses)
            System.out.println("\t=>" + sense.getSenseString());
        System.out.println();
        System.out.println("SYNSETS WITH \"" + lemma + "\"");
        List<BabelSynset> synsets =
                bn.getSynsets(languageToSearch, lemma, POS.NOUN,
                        includeRedirections, allowedSources);
        Collections.sort(synsets, new BabelSynsetComparator(lemma));
        for (BabelSynset synset : synsets)
            System.out.println(
                    "\t=>(" + synset.getId() +
                            ") SOURCE: " + synset.getSynsetSource() +
                            ") TYPE: " + synset.getSynsetType() +
                            "; WN SYNSET: " + synset.getWordNetOffsets() +
                            "; MAIN SENSE: " + synset.getMainSense() +
                            "; SENSES: " + synset.toString());
        System.out.println();
    }

    /**
     * A demo to explore the BabelNet graph.
     *
     * @param id
     * @throws IOException
     */
    public static void testGraph(String id) throws IOException {
        BabelNet bn = BabelNet.getInstance();
        BabelSynset synset = bn.getSynsetFromId(id);

        testGraph(synset);
    }

    /**
     * A demo to explore the BabelNet graph.
     *
     * @param lemma
     * @param language
     * @throws IOException
     */
    public static void testGraph(String lemma, Language language) throws IOException {
        BabelNet bn = BabelNet.getInstance();
        List<BabelSynset> synsets = bn.getSynsets(language, lemma);
        Collections.sort(synsets, new BabelSynsetComparator(lemma));

        for (BabelSynset synset : synsets) testGraph(synset);
    }

    /**
     * A demo to explore the BabelNet graph.
     *
     * @param synset
     * @throws IOException
     */
    public static void testGraph(BabelSynset synset) throws IOException {
        BabelNet bn = BabelNet.getInstance();
        List<BabelNetGraphEdge> successorsEdges = bn.getSuccessorEdges(synset.getId());

        System.out.println("SYNSET ID:" + synset.getId());
        System.out.println("# OUTGOING EDGES: " + successorsEdges.size());

        for (BabelNetGraphEdge edge : successorsEdges) {
            System.out.println("\tEDGE " + edge);
            System.out.println("\t" + bn.getSynsetFromId(edge.getTarget()).toString(Language.EN));
            System.out.println();
        }
    }

    /**
     * A demo to see the translations of a word.
     *
     * @param lemma
     * @param languageToSearch
     * @param languagesToPrint
     * @throws IOException
     */
    public static void testTranslations(String lemma, Language languageToSearch,
                                        Language... languagesToPrint) throws IOException {
        BabelNet bn = BabelNet.getInstance();

        List<Language> allowedLanguages = Arrays.asList(languagesToPrint);
        Multimap<Language, ScoredItem<String>> translations =
                bn.getTranslations(languageToSearch, lemma);
//        bn.getSynsetsFromWikipediaTitle()

        System.out.println("TRANSLATIONS FOR " + lemma);
        for (Language language : translations.keySet()) {
            if (allowedLanguages.contains(language))
                System.out.println("\t" + language + "=>" + translations.get(language));
        }
    }

    /**
     * A demo to see the glosses of a {@link BabelSynset} given its id.
     *
     * @param id
     * @throws IOException
     */
    public static void testGloss(String id) throws IOException {
        BabelNet bn = BabelNet.getInstance();
        BabelSynset synset = bn.getSynsetFromId(id);

        testGloss(synset);
    }

    /**
     * A demo to see the glosses of a word in a certain language
     *
     * @param lemma
     * @param language
     * @throws IOException
     */
    public static void testGloss(String lemma, Language language) throws IOException {
        BabelNet bn = BabelNet.getInstance();
        List<BabelSynset> synsets = bn.getSynsets(language, lemma);
        Collections.sort(synsets, new BabelSynsetComparator(lemma));

        for (BabelSynset synset : synsets) testGloss(synset);
    }

    /**
     * A demo to see the glosses of a {@link BabelSynset}
     *
     * @param synset
     * @throws IOException
     */
    public static void testGloss(BabelSynset synset) throws IOException {
        String id = synset.getId();
        BabelNet bn = BabelNet.getInstance();
        List<BabelGloss> glosses = bn.getGlosses(id);

        System.out.println("GLOSSES FOR SYNSET " + synset + " -- ID: " + id);
        for (BabelGloss gloss : glosses) {
            System.out.println(" * " + gloss.getLanguage() + " " + gloss.getSource() + " " +
                    gloss.getSourceSense() + "\n\t" + gloss.getGloss());
        }
        System.out.println();
    }

    public static void testImages(String lemma, Language language) throws IOException {
        BabelNet bn = BabelNet.getInstance();
        System.out.println("SYNSETS WITH word: \"" + lemma + "\"");
        List<BabelSynset> synsets = bn.getSynsets(language, lemma);
        Collections.sort(synsets, new BabelSynsetComparator(lemma));
        for (BabelSynset synset : synsets) {
            System.out.println("  =>(" + synset.getId() + ")" +
                    "  MAIN LEMMA: " + synset.getMainSense());
            for (BabelImage img : synset.getImages()) {
                System.out.println("\tIMAGE URL:" + img.getURL());
                System.out.println("\tIMAGE VALIDATED URL:" + img.getValidatedURL());
                System.out.println("\t==");
            }
            System.out.println("  -----");
        }
    }

    /**
     * The snippet contained in our WWW-12 demo paper
     */
    public static void www12Test() throws IOException {
        BabelNet bn = BabelNet.getInstance();
        System.out.println("SYNSETS WITH English word: \"bank\"");
        List<BabelSynset> synsets = bn.getSynsets(Language.EN, "bank");
        Collections.sort(synsets, new BabelSynsetComparator("bank"));
        for (BabelSynset synset : synsets) {
            System.out.print("  =>(" + synset.getId() + ") SOURCE: " + synset.getSynsetSource() +
                    "; TYPE: " + synset.getSynsetType() +
                    "; WN SYNSET: " + synset.getWordNetOffsets() + ";\n" +
                    "  MAIN LEMMA: " + synset.getMainSense() +
                    ";\n  IMAGES: " + synset.getImages() +
                    ";\n  CATEGORIES: " + synset.getCategories() +
                    ";\n  SENSES (German): { ");
            for (BabelSense sense : synset.getSenses(Language.DE))
                System.out.print(sense.toString() + " ");
            System.out.println("}\n  -----");
            Map<IPointer, List<BabelSynset>> relatedSynsets = synset.getRelatedMap();
            for (IPointer relationType : relatedSynsets.keySet()) {
                List<BabelSynset> relationSynsets = relatedSynsets.get(relationType);
                for (BabelSynset relationSynset : relationSynsets) {
                    System.out.println("    EDGE " + relationType.getSymbol() +
                            " " + relationSynset.getId() +
                            " " + relationSynset.toString(Language.EN));
                }
            }
            System.out.println("  -----");
        }
    }

    /**
     * A demo to test iterators.
     *
     * @param iterator
     */
    public static <T> void testIterator(BabelIterator<T> iterator) {
        int counter = 0;
        while (iterator.hasNext()) {
            System.out.println(counter + ". " + iterator.next().toString());
            counter++;
        }
    }

    /**
     * Just for testing
     *
     * @param args
     */
    static public void main(String[] args) {
        try {
//            testImages("zero",Language.EN);
//            testImages("Zwei",Language.DE);
//            imageExport(false, true);
            imageExport(false, false);
//            imageExport(true);
            if (1 > 0)
                return;
            BabelNet bn = BabelNet.getInstance();
            List<BabelSynset> synsets = bn.getSynsets(Language.EN, "brim");
            int offsetFromSynsetID = BabelSynset.getOffsetFromSynsetID("102902079");
            BabelSynset synsetFromId = bn.getSynsetFromId("" + offsetFromSynsetID);
            BabelSynset synsetFromId2 = bn.getSynsetFromId("102902079");
//            bn:00013135n ==       02902250n

            // UNCOMMENT TO TEST THE WWW-12 SNIPPET
//			www12Test();

            // UNCOMMENT TO TEST THE IMAGES
//			testImages("balloon", Language.EN);

            // UNCOMMENT TO TEST THE ITERATORS
//			BabelNet bn = BabelNet.getInstance();
//			testIterator(bn.getOffsetIterator());
//			testIterator(bn.getSynsetIterator());
//			testIterator(bn.getLexiconIterator());

            // UNCOMMENT TO TEST THE LEXICON
//			System.out.println("===============TESTING BABELNET DICT===============\n");
//            =>(bn:00043620n) SOURCE: WIKIWN) TYPE: Concept; WN SYNSET: [06632511n]; MAIN SENSE: hello#n#1; SENSES: { WIKI:DE:Hallo, WIKI:DE:Howdy, WIKITR:DE:hallo_0.80952_17_21, WNTR:DE:hallo_1.00000_0_0, WNTR:DE:hullo_1.00000_0_0 }
//            =>(bn:00003002n) SOURCE: WIKIWN) TYPE: Named Entity; WN SYNSET: [09078231n]; MAIN SENSE: Hawaii#n#1; SENSES: { WIKIRED:DE:US-HI, WIKIRED:DE:Hawai'i, WIKIRED:DE:Aloha_State, WIKI:DE:Hawaii, WIKIRED:DE:Hawaiʻi, WNTR:DE:aloha_state_1.00000_0_0, WNTR:DE:hawai'i_1.00000_0_0, WNTR:DE:hawaii_1.00000_9_9, OMWIKI:DE:Hawaii }
//            =>(bn:03298690n) SOURCE: WIKI) TYPE: Concept; WN SYNSET: []; MAIN SENSE: WIKI:EN:Hi_(kana); SENSES: { }
//            =>(bn:00536859n) SOURCE: WIKI) TYPE: Named Entity; WN SYNSET: []; MAIN SENSE: WIKI:EN:Hi_(magazine); SENSES: { }
//            =>(bn:03548040n) SOURCE: WIKI) TYPE: Named Entity; WN SYNSET: []; MAIN SENSE: WIKI:EN:Hi_(Ofra_Haza_song); SENSES: { }
            // MATCHES Wordnet 3 id!!!   09078231n <-> 109078231
//            62351	13968	109078231	86382	1	0	9	hawaii%1:15:00::
            String[] strings = new String[]{
                    "brim",
//					"house",
//					"car",
//					"account"
            };
            for (String test : strings)
                testDictionary(test, Language.EN, true, Language.DE);

//            translateAll();
//			System.out.println("=====================DONE=====================");

            // UNCOMMENT TO TEST THE GRAPH
//			System.out.println("===============TESTING BABELNET GRAPH===============\n");
//			testGraph("bank", Language.EN);
//			testGraph("bn:00000010n");
//			System.out.println("=====================DONE=====================");

//			UNCOMMENT TO TEST THE TRANSLATIONS
//			System.out.println("===============TESTING BABELNET TRANSLATIONS===============\n");
//            Set<Language> languages = new HashSet<Language>();  // BabelNetConfiguration.getInstance().getBabelLanguages();
//            languages.add(Language.DE);
////            testTranslations("apple", Language.EN, languages.toArray(new Language[languages.size()]));
//            testTranslations("hope", Language.EN, languages.toArray(new Language[languages.size()]));
//			System.out.println("=====================DONE=====================");

            // UNCOMMENT TO TEST THE GLOSSES
//			System.out.println("===============TESTING BABELNET GLOSSES===============\n");
//			testGloss("play", Language.EN);
//			testGloss("bn:00000010n");
//			System.out.println("=====================DONE=====================");

        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    private static void imageExport(boolean all, boolean thumbs) throws IOException {
        BabelNet babelNet = BabelNet.getInstance();
        BabelSynsetIterator synsetIterator = babelNet.getSynsetIterator();
        HashMap done = new HashMap();
        while (synsetIterator.hasNext()) {
            try {
                BabelSynset synset = synsetIterator.next();
                if (synset.getImages().size() == 0) continue;
//            for (String sensekey : synset.getWordNetOffsets()) {
                List<String> wordNetOffsets = synset.getWordNetOffsets();
                if(wordNetOffsets.size()==0) continue;
                String sensekey = wordNetOffsets.get(0);
//            BabelImage img = synset.getImages().get(0);
                String word = synset.getMainSense().replaceAll("#.*", "");
                List<BabelSense> senses = synset.getSenses(Language.DE);
//                Collections.sort(senses, new BabelSenseComparator());
//                String lastWord = "";
                for (BabelSense sense : senses) {
                    word = sense.getLemma();
                    if(done.containsKey(word.toLowerCase()))continue;
                    done.put(word.toLowerCase(), true);
//                    if (lastWord.equals(word)) continue;
//                    lastWord = word;
//                    String word = translate(synset);
                    for (BabelImage img : synset.getImages()) {
                        if (img.isBadImage()) continue;
//                    if(img.getValidatedURL())
                        boolean isJPG = img.getName().contains("jpg") || img.getName().contains("JPG") || img.getName().contains("png");
//                        if (!all && !isJPG) continue;  // NO SVG etc
                        String url;
                        if (thumbs)
//                        url = img.getValidatedThumbURL();
                            url = img.getThumbURL();
                        else
//                        url = img.getValidatedURL();
                            url = img.getURL();
                        if (url == null) continue;
                        int length = url.length();
                        int length1 = img.getName().length();
                        int i = length - length1 - 6;
                        String md5 = url.substring(i, i + 6);
                        System.out.println(word + "\t" + md5 + "\t" + img.getName() + "\t" + sensekey + "\t" + url);
                        //            System.out.println(synset.getId() + "\t" + synset.getMainSense() + "\t" + img.getName());
                        if (!all) break;
                    }
                }
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }

}

    private static String translate(BabelSynset synset) {
        String translation = null;
        POS pos = synset.getPOS();
        List<BabelSense> senses = synset.getSenses(Language.DE);
        if (senses.size() == 0) return translation;
        Collections.sort(senses, new BabelSenseComparatorDE());
        BabelSense sense = senses.get(0);
        translation = sense.toString();
        if (pos.equals(POS.NOUN))
            translation = translation.substring(0, 1).toUpperCase() + translation.substring(1);
        return translation;
    }

    private static void translateAll() throws IOException {
        BabelNet babelNet = BabelNet.getInstance();
        BabelSynsetIterator synsetIterator = babelNet.getSynsetIterator();
//        BabelSynsetIterator synsetIterator = babelNet.getSynsetIterator();
        while (synsetIterator.hasNext()) {
            BabelSynset synset = synsetIterator.next();
            List<String> wordNetOffsets = synset.getWordNetOffsets();
            String wordnet_id = wordNetOffsets.get(0);// babelId=synset.getId();
            String mainSense = synset.getMainSense().replaceAll("#.*", "");
            POS pos = synset.getPOS();
            List<BabelSense> senses = synset.getSenses(Language.DE);
            Collections.sort(senses, new BabelSenseComparatorDE());
            String translation = mainSense;
            if (senses.size() == 0) {
//                System.out.println(mainSense + " ????????????");
                continue;
            } else {
                BabelSense sense = senses.get(0);
                translation = sense.toString();
            }
            if (pos.equals(POS.NOUN))
                translation = translation.substring(0, 1).toUpperCase() + translation.substring(1);
            List<String> list = new ArrayList<String>();
            for (Object o : senses.toArray()) {
                BabelSense s = (BabelSense) o;
                if (s.equals(senses)) continue; //senses.remove(s);
//                if(list.contains(s.toString()) )// senses.remove(s);
                String s1 = s.toString();
                if (pos.equals(POS.NOUN)) s1 = s1.substring(0, 1).toUpperCase() + s1.substring(1);
                if (!list.contains(s1) && !s1.equals(translation))
                    list.add(s1);
            }
//            Collection<BabelSense> babelSenses = synset.getTranslations().get(synset.getSenses(Language.EN).get(0));
//                sense.getLanguage())) continue;
//            if(mainSense.contains("#2"))

            System.out.println(wordnet_id + "\t" + mainSense + "\t" + translation + "\t" + list);//+ "\n"
//            System.out.println(mainSense + " "+sense+ " "+wordNetOffsets+" "+glosses+"\n" + senses+"\n");
//            for (BabelSense tr : babelSenses)
//                    if(tr.getLanguage().equals(Language.DE))
//                        System.out.println(">>>>>>>>>>>"+tr);
        }

    }
}
