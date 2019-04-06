# SS-CE

Μπορεί να βρεθεί και στο δικό μου [Github](https://github.com/Sima214/SS-CE).

## Building

Για την κατασκευή των εκτελέσιμων αρχείων απαιτείται python(3.+), cmake(3.9.+) και mingw-w64(5.3+). Ή μπορείται απλά να τρέξετε το `build.bat`, το οποίο απαιτεί να έχετε μόνο εγκατεστημένη την 64bit έκδοση τοu 7z (το δοκίμασα στους υπολογιστές του εργαστηρίου). Αυτό παράγει ένα `package.tag.xz`, το οποίο περιέχει τα αρχεία που έγιναν compile.

## Εκτέλεση εργασίας

Τα εκτελέσιμα αρχεία που σας ενδιαφέρουν είναι τα `test_ai_search_uninformed_bfs`, `test_ai_search_uninformed_dfs`, `test_ai_search_informed_bestfirst`. Απαιτούνται κάποια dll, τα οποία μπορείται να τα βρείτε κάπου στον φάκελο(`tools/lib` και `package.tar.xz/lib`).

## Documentation

Δεν είναι αρκετά έτοιμο για να το βγάλω στον κόσμο, οπότε θα σας αφήσω μια alpha έκδοση μήπως σας βοηθήσει.

## Πηγαίος κώδικας

### src/structures/Interface.h

Περιγραφή ενός 'αφηρημένου' τύπου δεδομένων στη C.

Αυτό περισσότερο καλύπτει την έλλειψη του `operator`.

### src/ai/search/SearchProblem.h

Αυτό το αρχείο περιγράφει την κλάση/interface ενός προβλήματος αναζήτησης.

### src/ai/search/uninformed/BFS.c

Αλγόριθμος BFS.

### src/ai/search/uninformed/DFS.c

Αλγόριθμος DFS.

### src/ai/search/informed/BestFirst.c

Αλγόριθμος Best First Search.

### test/ai_search.h

Εφαρμογή του [SearchProblem](#src/ai/search/SearchProblem.h) για το συγκεκριμένο πρόβλημα.

Η δομή των καταστάσεων δεν είναι προφανής, οπότε θα την αναλύσω.

Έστω ένας πίνακας 2x2:

| | |
|-|-|
|1|2|
| |3|

Αυτό αποθηκεύεται στην μνήμη ως ένας μονοδιάστατος πίνακας `{0, 1, 3, 2}`. Η αντιστοίχιση είναι έτσι ώστε η τελική κατάσταση να έχει όλα τα στοιχεία σε αύξουσα σειρά.
(Zero based indexing and empty space is the biggest index).

Αλλά δεν τελειώνει εδώ. Στην αρχή (πριν τον πίνακα) αποθηκεύται μια τιμή τύπου `size_t`, η οποία κρατάει το 'βάθος' της κατάστασης.

Αλλά τι μας λύπει;

Πώς κρατάμε την λύση φυσικά!

Πόσες πιθανές κινήσεις έχουμε;

4

Πόσα bits χρειαζόμαστε για να αποθηκεύσουμε;

2 bits
(ή 4 κινήσεις ανά byte)

Οπότε μετά από το array, έχουμε ένα άλλο array από bytes, που το κάθε στοιχείο κρατάει 4 κινήσεις.

Τελικά έχουμε:

```C
struct HistoryShell {
    unsigned int first : 2;
    unsigned int second : 2;
    unsigned int third : 2;
    unsigned int fourth : 2;
}/*packed to 1 byte*/;

/**
 * Αυτό δεν κάνει compile, αφού τα μεγέθη δεν είναι σταθερά.
 */
struct VariableState {
    size_t history_size;
    uint8_t table[n*n];
    HistoryShell history[(n*n*DEPTH_FACTOR)/4];
};
```

όπου `n` το μέγεθος του πίνακα σε μια πλευρά, και `DEPTH_FACTOR` μια σταθερά που ορίζει το μέγιστο βάθος.

### test/ai_search_uninformed_dfs.c

Συνδυασμός του [BFS](#src/ai/search/uninformed/BFS.c) με το [AISearch](test/ai_search.h).

### test/ai_search_uninformed_bfs.c

Συνδυασμός του [DFS](#src/ai/search/uninformed/DFS.c) με το [AISearch](test/ai_search.h).

### test/ai_search_informed_bestfirst.c

Συνδυασμός του [BestFS](#src/ai/search/uninformed/BestFirst.c) με το [AISearch](test/ai_search.h).

## Λειτουργεί;

Μόνο για συγκεκριμένες εισόδους.
Το ΗashSet και SortedArray φαίνεται ότι έχουν bugs, τα οποία δεν ανιχνέυτηκαν από τα αρχικά tests...