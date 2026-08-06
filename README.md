# Readability Analyzer

A desktop application that measures how hard a piece of writing is to read.

Open a text file and it reports the standard readability scores (Flesch Reading Ease, Flesch-Kincaid grade level, and the Gunning Fog index) along with word, sentence, syllable, and unique-word counts. A checkbox toggles the text between plain view and an HTML rendering colored by part of speech. The syllable-split view is computed by the same engine and exercised by the console demo and test suite, though the GUI doesn't currently expose a toggle for it.

Written in C++17 with Qt 6. Tokenization runs on a thread pool so large files stay fast, and all file loading happens off the UI thread so the window never freezes.

## What it measures

Every readability formula depends on the same three counts: words, sentences, and syllables. Syllables are the difficult one, since English has no reliable rule for them. The syllabizer works by reducing each word to a vowel/consonant pattern and then splitting on that, with a list of common prefixes and suffixes handling the cases the pattern gets wrong.

From those counts it computes:

| Score | What it means |
|---|---|
| Flesch Reading Ease | 0-100, higher is easier. Around 60-70 is plain English; below 30 is dense academic prose |
| Flesch-Kincaid Grade Level | US school grade needed to read the text |
| Gunning Fog Index | Grade level, weighted toward "hard words" of three or more syllables |

It also reports character, word, unique word, sentence, syllable, and hard word counts, and can find the most frequent repeated phrases of a given length.

Each word is looked up in a lexicon of about 74,000 English words to get its part of speech and whether it is a stopword. That is what drives the colorized view.

## Using it

The application opens with a file browser. Click a text file and it opens in its own window with the text, a grid of stat cards, and a checkbox that switches between plain text and the part-of-speech colorized view.

Statistics fill in as they are computed rather than all at once, so even a multi-megabyte file stays responsive while it loads.

## Building

You will need CMake 3.16 or newer, a C++17 compiler, and Qt 6 (Widgets, Core, Gui, and Test). Doxygen is optional.

```bash
cmake -B build
cmake --build build
```

That builds three executables:

| Target | Purpose |
|---|---|
| `guimain` | the desktop application |
| `main` | console demo that analyzes a file at several thread counts |
| `testproject` | the test suite |

Run the application from the repository root, since the lexicon is loaded from `text/US English word info.csv` relative to the working directory:

```bash
./build/guimain
```

Sample documents are in `text/`.

### Tests

```bash
./build/testproject
```

The suite covers the syllabizer, document statistics, token behavior, the readability formulas against known reference values, and both GUI windows. GUI tests run offscreen so they work without a display.

### Documentation

```bash
cmake --build build --target docs
```

Generates Doxygen API documentation into `html/`.

## How it works

### Parallel tokenization

The expensive work is per-word: syllabifying each token and looking it up in the lexicon. `Document::extracttokens(numthreads)` spreads that across a pool of worker threads.

```
                  work queue
   document  ───►  (word +   ───►  worker 1  ─┐
   scanned         offset)   ───►  worker 2  ─┤
   into words                ───►  worker N  ─┤
                                              ▼
                                        result queue
                                              │
                                   sort by original offset
                                              │
                                              ▼
                              tokens, HTML, syllabized text
```

The main thread scans the raw text into (word, byte offset) pairs and pushes them onto a work queue. Each worker pulls a word off, syllabifies it, looks up its lexicon entry, and pushes the finished token onto a result queue. One empty-string sentinel per worker signals shutdown.

Threads finish out of order, so every token carries the byte offset it came from and the results get sorted back into document order before assembly. The output is identical no matter how many threads are used. The console demo checks this by running the same file at 1, 2, 4, and 8 threads and comparing the results.

### The queue

Both queues are `ThreadSafeQueue<T>`, a small template over `std::queue` guarded by a `std::mutex` and a `std::condition_variable`. `pop()` waits on the condition variable until an item arrives, so idle workers block instead of spinning. Copy and assignment are deleted, since a queue shared across threads should never be duplicated by accident.

The mutex is `mutable` so that `empty()` and `size()` can be const while still locking.

### Keeping the interface responsive

Threading the tokenizer makes analysis fast, but reading the file and driving the analysis still cannot happen on the UI thread. Each `DocumentWindow` owns a `QThread` and a `DocumentWorker` moved onto it with `moveToThread()`. The window emits `startLoad(filename)`, and the worker emits `contentsReady`, `statsReady`, `htmlReady`, and `documentLoadDone` as each stage completes.

Qt delivers those signals back to the UI thread through queued connections, so the window updates progressively without blocking the event loop and without a worker ever touching a widget directly.

### The lexicon

`Lexicon` is a Meyers singleton, a function-local static that C++11 guarantees is initialized exactly once and thread-safely. It reads the CSV into an `unordered_map`, which gives constant-time lookups while many worker threads query it at the same time.

## Files

```
Document.*           document model, statistics, readability, parallel tokenization
Token.*              one word: syllable breaks, part of speech, source position
Syllabizer.*         English syllable decomposition
Lexicon.*            singleton word database
ThreadSafeQueue.hpp  blocking queue used by the thread pool

MainWindow.*         file browser
DocumentWindow.*     document view, statistics panel, HTML toggle
DocumentWorker.*     runs loading and analysis off the UI thread

main.cpp             console demo
guimain.cpp          application entry point
ProjectTest.*        test suite
text/                lexicon and sample documents
```

## Credits

`Syllabizer.cpp` and `Syllabizer.hpp` were written by Creed Jones and are included with his original attribution intact. Everything else is my own work.

`catch.hpp` is the vendored [Catch2](https://github.com/catchorg/Catch2) single-header test framework.

The word list in `text/US English word info.csv` is derived from [SUBTLEX-US](https://www.ugent.be/pp/experimentele-psychologie/en/research/documents/subtlexus) (Brysbaert & New), which provides the frequency and part-of-speech data behind the stopword and POS lookups.

Sample text in `text/` is public domain: the Gettysburg Address and Arthur Conan Doyle's Sherlock Holmes.
