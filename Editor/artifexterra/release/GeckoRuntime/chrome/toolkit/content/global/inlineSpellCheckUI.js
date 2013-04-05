//@line 38 "/g/mozilla/toolkit/content/inlineSpellCheckUI.js"

var InlineSpellCheckerUI = {
  mOverMisspelling: false,
  mMisspelling: "",
  mMenu: null,
  mSpellSuggestions: [], // text of words
  mSuggestionItems: [],  // menuitem nodes
  mDictionaryMenu: null,
  mDictionaryNames: [], // internal dictionary names (e.g. "en-US")
  mDictionaryItems: [],
  mLanguageBundle: null, // language names
  mRegionBundle: null, // region names

  // Call this function to initialize for a given editor
  init: function(aEditor)
  {
    this.uninit();
    this.mEditor = aEditor;
    try {
      this.mInlineSpellChecker = this.mEditor.inlineSpellChecker;
      // note: this might have been NULL if there is no chance we can spellcheck
    } catch(e) {
      this.mInlineSpellChecker = null;
    }
  },

  // call this to clear state
  uninit: function()
  {
    this.mInlineSpellChecker = null;
    this.mOverMisspelling = false;
    this.mMisspelling = "";
    this.mMenu = null;
    this.mSpellSuggestions = [];
    this.mSuggestionItems = [];
    this.mDictionaryMenu = null;
    this.mDictionaryNames = [];
    this.mDictionaryItems = [];
  },

  // for each UI event, you must call this function, it will compute the
  // word the cursor is over
  initFromEvent: function(rangeParent, rangeOffset)
  {
    this.mOverMisspelling = false;

    if (! this.mInlineSpellChecker)
      return;

    var selcon = this.mEditor.selectionController;
    var spellsel = selcon.getSelection(selcon.SELECTION_SPELLCHECK);
    if (spellsel.rangeCount == 0)
      return; // easy case - no misspellings

    var range = this.mInlineSpellChecker.getMispelledWord(rangeParent,
                                                          rangeOffset);
    if (! range)
      return; // not over a misspelled word

    this.mMisspelling = range.toString();
    this.mOverMisspelling = true;
    this.mWordNode = rangeParent;
    this.mWordOffset = rangeOffset;
  },

  // returns false if there should be no spellchecking UI enabled at all, true
  // means that you can at least give the user the ability to turn it on.
  get canSpellCheck()
  {
    // inline spell checker objects will be created only if there are actual
    // dictionaries available
    return (this.mInlineSpellChecker != null);
  },

  // Whether spellchecking is enabled in the current box
  get enabled()
  {
    return (this.mInlineSpellChecker &&
            this.mInlineSpellChecker.enableRealTimeSpell);
  },
  set enabled(isEnabled)
  {
    if (this.mInlineSpellChecker)
      this.mEditor.QueryInterface(Components.interfaces.nsIEditor_MOZILLA_1_8_BRANCH).setSpellcheckUserOverride(isEnabled);
  },

  // returns true if the given event is over a misspelled word
  get overMisspelling()
  {
    return this.mOverMisspelling;
  },

  // this prepends up to "maxNumber" suggestions at the given menu position
  // for the word under the cursor. Returns the number of suggestions inserted.
  addSuggestionsToMenu: function(menu, insertBefore, maxNumber)
  {
    if (! this.mInlineSpellChecker || ! this.mOverMisspelling)
      return 0; // nothing to do

    var spellchecker = this.mInlineSpellChecker.spellChecker;
    if (! spellchecker.CheckCurrentWord(this.mMisspelling))
      return 0;  // word seems not misspelled after all (?)

    this.mMenu = menu;
    this.mSpellSuggestions = [];
    this.mSuggestionItems = [];
    for (var i = 0; i < maxNumber; i ++) {
      var suggestion = spellchecker.GetSuggestedWord();
      if (! suggestion.length)
        break;
      this.mSpellSuggestions.push(suggestion);

      var item = document.createElement("menuitem");
      this.mSuggestionItems.push(item);
      item.setAttribute("label", suggestion);
      item.setAttribute("value", suggestion);
      // this function thing is necessary to generate a callback with the
      // correct binding of "val" (the index in this loop).
      var callback = function(me, val) { return function(evt) { me.replaceMisspelling(val); } };
      item.addEventListener("command", callback(this, i), true);
      item.setAttribute("class", "spell-suggestion");
      menu.insertBefore(item, insertBefore);
    }
    return this.mSpellSuggestions.length;
  },

  // undoes the work of addSuggestionsToMenu for the same menu
  // (call from popup hiding)
  clearSuggestionsFromMenu: function()
  {
    for (var i = 0; i < this.mSuggestionItems.length; i ++) {
      this.mMenu.removeChild(this.mSuggestionItems[i]);
    }
    this.mSuggestionItems = [];
  },

  // returns the number of dictionary languages. If insertBefore is NULL, this
  // does an append to the given menu
  addDictionaryListToMenu: function(menu, insertBefore)
  {
    this.mDictionaryMenu = menu;
    this.mDictionaryNames = [];
    this.mDictionaryItems = [];

    if (! this.mLanguageBundle) {
      // create the bundles for language and region
      var bundleService = Components.classes["@mozilla.org/intl/stringbundle;1"]
                                    .getService(Components.interfaces.nsIStringBundleService);
      this.mLanguageBundle = bundleService.createBundle(
          "chrome://global/locale/languageNames.properties");
      this.mRegionBundle = bundleService.createBundle(
          "chrome://global/locale/regionNames.properties");
    }

    if (! this.mInlineSpellChecker || ! this.enabled)
      return 0;
    var spellchecker = this.mInlineSpellChecker.spellChecker;
    var o1 = {}, o2 = {};
    spellchecker.GetDictionaryList(o1, o2);
    var list = o1.value;
    var listcount = o2.value;
    var curlang = spellchecker.GetCurrentDictionary();
    var isoStrArray;

    for (var i = 0; i < list.length; i ++) {
      // get the display name for this dictionary
      isoStrArray = list[i].split("-");
      var displayName = "";
      if (this.mLanguageBundle && isoStrArray[0]) {
        try {
          displayName = this.mLanguageBundle.GetStringFromName(isoStrArray[0].toLowerCase());
        } catch(e) {} // ignore language bundle errors
        if (this.mRegionBundle && isoStrArray[1]) {
          try {
            displayName += " / " + this.mRegionBundle.GetStringFromName(isoStrArray[1].toLowerCase());
          } catch(e) {} // ignore region bundle errors
          if (isoStrArray[2])
            displayName += " (" + isoStrArray[2] + ")";
        }
      }

      // if we didn't get a name, just use the raw dictionary name
      if (displayName.length == 0)
        displayName = list[i];

      this.mDictionaryNames.push(list[i]);
      var item = document.createElement("menuitem");
      item.setAttribute("label", displayName);
      item.setAttribute("type", "checkbox");
      this.mDictionaryItems.push(item);
      if (curlang == list[i]) {
        item.setAttribute("checked", "true");
      } else {
        var callback = function(me, val) { return function(evt) { me.selectDictionary(val); } };
        item.addEventListener("command", callback(this, i), true);
      }
      if (insertBefore)
        menu.insertBefore(item, insertBefore);
      else
        menu.appendChild(item);
    }
    return list.length;
  },

  // undoes the work of addDictionaryListToMenu for the menu
  // (call on popup hiding)
  clearDictionaryListFromMenu: function()
  {
    for (var i = 0; i < this.mDictionaryItems.length; i ++) {
      this.mDictionaryMenu.removeChild(this.mDictionaryItems[i]);
    }
    this.mDictionaryItems = [];
  },

  // callback for selecting a dictionary
  selectDictionary: function(index)
  {
    if (! this.mInlineSpellChecker || index < 0 || index >= this.mDictionaryNames.length)
      return;
    var spellchecker = this.mInlineSpellChecker.spellChecker;
    spellchecker.QueryInterface(Components.interfaces.nsIEditorSpellCheck_MOZILLA_1_8_BRANCH);
    spellchecker.SetCurrentDictionary(this.mDictionaryNames[index]);
    spellchecker.saveDefaultDictionary();
    this.mInlineSpellChecker.spellCheckRange(null); // causes recheck
  },

  // callback for selecting a suggesteed replacement
  replaceMisspelling: function(index)
  {
    if (! this.mInlineSpellChecker || ! this.mOverMisspelling)
      return;
    if (index < 0 || index >= this.mSpellSuggestions.length)
      return;
    this.mInlineSpellChecker.replaceWord(this.mWordNode, this.mWordOffset,
                                         this.mSpellSuggestions[index]);
  },

  // callback for enabling or disabling spellchecking
  toggleEnabled: function()
  {
    this.mEditor.QueryInterface(Components.interfaces.nsIEditor_MOZILLA_1_8_BRANCH).setSpellcheckUserOverride(!this.mInlineSpellChecker.enableRealTimeSpell);
  },

  // callback for adding the current misspelling to the user-defined dictionary
  addToDictionary: function()
  {
    this.mInlineSpellChecker.addWordToDictionary(this.mMisspelling);
  }
};
