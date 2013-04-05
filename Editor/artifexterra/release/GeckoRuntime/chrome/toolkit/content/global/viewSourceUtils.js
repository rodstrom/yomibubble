//@line 37 "/g/mozilla/toolkit/components/viewsource/content/viewSourceUtils.js"

/*
 * To keep the global namespace safe, don't define global variables and 
 * functions in this file.
 *
 * This file requires contentAreaUtils.js
*/

var gViewSourceUtils = {

  mnsIWebBrowserPersist: Components.interfaces.nsIWebBrowserPersist,
  mnsIWebProgress: Components.interfaces.nsIWebProgress,
  mnsIWebPageDescriptor: Components.interfaces.nsIWebPageDescriptor,

  // Opens the interval view source viewer
  openInInternalViewer: function(aURL, aPageDescriptor, aDocument)
  {
    // try to open a view-source window while inheriting the charset (if any)
    var charset = null;
    if (aDocument)
      charset = "charset=" + aDocument.characterSet;
    openDialog("chrome://global/content/viewSource.xul",
               "_blank",
               "all,dialog=no",
               aURL, charset, aPageDescriptor);
  },

  // aCallBack is a function accepting two arguments - result (true=success) and a data object
  // It defaults to openInInternalViewer if undefined.
  openInExternalEditor: function(aURL, aPageDescriptor, aDocument, aCallBack)
  {
    var data = {url: aURL, pageDescriptor: aPageDescriptor, doc: aDocument};

    try {
      var editor = this.getExternalViewSourceEditor();    
      if (!editor) {
        this.handleCallBack(aCallBack, false, data);
        return;
      }

      // make a uri
      var ios = Components.classes["@mozilla.org/network/io-service;1"]
                          .getService(Components.interfaces.nsIIOService);
      var charset = aDocument ? aDocument.characterSet : null;
      var uri = ios.newURI(aURL, charset, null);
      data.uri = uri;

      var path;
      var contentType = aDocument ? aDocument.contentType : null;
      if (uri.scheme == "file") {    
        // it's a local file; we can open it directly
        path = uri.QueryInterface(Components.interfaces.nsIFileURL).file.path;
        editor.run(false, [path], 1);
        this.handleCallBack(aCallBack, true, data);
      } else {
        // set up the progress listener with what we know so far
        this.viewSourceProgressListener.editor = editor;
        this.viewSourceProgressListener.callBack = aCallBack;
        this.viewSourceProgressListener.data = data;      
        if (!aPageDescriptor) {
          // without a page descriptor, loadPage has no chance of working. download the file.
          var file = this.getTemporaryFile(uri, aDocument, contentType);
          this.viewSourceProgressListener.file = file;

          var webBrowserPersist = Components
                                  .classes["@mozilla.org/embedding/browser/nsWebBrowserPersist;1"]
                                  .createInstance(this.mnsIWebBrowserPersist);
          // the default setting is to not decode. we need to decode.
          webBrowserPersist.persistFlags = this.mnsIWebBrowserPersist.PERSIST_FLAGS_REPLACE_EXISTING_FILES;
          webBrowserPersist.progressListener = this.viewSourceProgressListener;
          webBrowserPersist.saveURI(uri, null, null, null, null, file);
        } else {
          // we'll use nsIWebPageDescriptor to get the source because it may not have to refetch
          // the file from the server
          var webShell = Components.classes["@mozilla.org/webshell;1"].createInstance();
          this.viewSourceProgressListener.webShell = webShell;
          var progress = webShell.QueryInterface(this.mnsIWebProgress);
          progress.addProgressListener(this.viewSourceProgressListener,
                                       this.mnsIWebProgress.NOTIFY_STATE_DOCUMENT);
          var pageLoader = webShell.QueryInterface(this.mnsIWebPageDescriptor);    
          pageLoader.loadPage(aPageDescriptor, this.mnsIWebPageDescriptor.DISPLAY_AS_SOURCE);
        }
      }
    } catch (ex) {
      // we failed loading it with the external editor.
      this.handleCallBack(aCallBack, false, data);
      return;
    }
  },

  // Default callback - opens the internal viewer if the external editor failed
  internalViewerFallback: function(result, data)
  {
    if (!result) {
      this.openInInternalViewer(data.url, data.pageDescriptor, data.doc);
    }
  },

  // Calls the callback, keeping in mind undefined or null values.
  handleCallBack: function(aCallBack, result, data)
  {
    // ifcallback is undefined, default to the internal viewer
    if (aCallBack === undefined) {
      this.internalViewerFallback(result, data);
    } else if (aCallBack) {
      aCallBack(result, data);
    }
  },

  // Returns nsIProcess of the external view source editor or null
  getExternalViewSourceEditor: function()
  {
    var editor = null;
    var viewSourceAppPath = null;
    try {
      var prefs = Components.classes["@mozilla.org/preferences-service;1"]
                            .getService(Components.interfaces.nsIPrefBranch);
      var prefPath = prefs.getCharPref("view_source.editor.path");
      if (prefPath.length > 0) {
        viewSourceAppPath = Components.classes["@mozilla.org/file/local;1"]
                                      .createInstance(Components.interfaces.nsILocalFile);
        viewSourceAppPath.initWithPath(prefPath);
        // it's gotta be executable
        if (viewSourceAppPath.exists() && viewSourceAppPath.isExecutable()) {
          editor = Components.classes['@mozilla.org/process/util;1']
                             .createInstance(Components.interfaces.nsIProcess);
          editor.init(viewSourceAppPath);
        }
      }
    }
    catch (ex) {
      dump(ex);
    }
    return editor;
  },

  viewSourceProgressListener: {

    mnsIWebProgressListener: Components.interfaces.nsIWebProgressListener,

    QueryInterface: function(aIID) {
     if (aIID.equals(this.mnsIWebProgressListener) ||
         aIID.equals(Components.interfaces.nsISupportsWeakReference) ||
         aIID.equals(Components.interfaces.nsISupports))
       return this;
     throw Components.results.NS_NOINTERFACE;
    },

    destroy: function() {
        this.webShell = null;
        this.editor = null;
        this.callBack = null;
        this.data = null;
        this.file = null;
    },

    onStateChange: function(aProgress, aRequest, aFlag, aStatus) {
      // once it's done loading...
      if ((aFlag & this.mnsIWebProgressListener.STATE_STOP) && aStatus == 0) {
        try {
          if (!this.file) {
            // it's not saved to file yet, it's in the webshell

            // get a temporary filename using the attributes from the data object that
            // openInExternalEditor gave us
            this.file = gViewSourceUtils.getTemporaryFile(this.data.uri, this.data.doc, 
                                                          this.data.doc.contentType);

            // we have to convert from the source charset.
            var webNavigation = this.webShell.QueryInterface(Components.interfaces.nsIWebNavigation);
            var foStream = Components.classes["@mozilla.org/network/file-output-stream;1"]
                                     .createInstance(Components.interfaces.nsIFileOutputStream);
            foStream.init(this.file, 0x02 | 0x08 | 0x20, 0664, 0); // write | create | truncate
            var coStream = Components.classes["@mozilla.org/intl/converter-output-stream;1"]
                                     .createInstance(Components.interfaces.nsIConverterOutputStream);
            coStream.init(foStream, this.data.doc.characterSet, 0, null);

            // write the source to the file
            coStream.writeString(webNavigation.document.body.textContent);
          
            // clean up
            coStream.close();
            foStream.close();
          }
          // fire up the editor
          this.editor.run(false, [this.file.path], 1);

          gViewSourceUtils.handleCallBack(this.callBack, true, this.data);
        } catch (ex) {
          // we failed loading it with the external editor.
          gViewSourceUtils.handleCallBack(this.callBack, false, this.data);
        } finally {
          this.destroy();
        }
      }
      return 0;
    },

    onLocationChange: function() {return 0;},
    onProgressChange: function() {return 0;},
    onStatusChange: function() {return 0;},
    onSecurityChange: function() {return 0;},
    onLinkIconAvailable: function() {return 0;},

    webShell: null,
    editor: null,
    callBack: null,
    data: null,
    file: null
  },

  // returns an nsIFile for the passed document in the system temp directory
  getTemporaryFile: function(aURI, aDocument, aContentType) {
    var fileLocator = Components.classes["@mozilla.org/file/directory_service;1"]
                                .getService(Components.interfaces.nsIProperties);
    var tempFile = fileLocator.get("TmpD", Components.interfaces.nsIFile);
    var fileName = getDefaultFileName(null, aURI, aDocument, aContentType);
    var extension = getDefaultExtension(fileName, aURI, aContentType);
    var leafName = getNormalizedLeafName(fileName, extension);
    tempFile.append(leafName);
    return tempFile;
  }
}
