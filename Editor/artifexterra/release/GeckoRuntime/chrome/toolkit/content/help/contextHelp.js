//@line 38 "/g/mozilla/toolkit/components/help/content/contextHelp.js"

//@line 41 "/g/mozilla/toolkit/components/help/content/contextHelp.js"
var helpFileURI;

//@line 46 "/g/mozilla/toolkit/components/help/content/contextHelp.js"
function openHelp(topic, contentPack)
{
//@line 50 "/g/mozilla/toolkit/components/help/content/contextHelp.js"
  helpFileURI = contentPack || helpFileURI;

//@line 53 "/g/mozilla/toolkit/components/help/content/contextHelp.js"
  var topWindow = locateHelpWindow(helpFileURI);

  if ( topWindow ) {
//@line 57 "/g/mozilla/toolkit/components/help/content/contextHelp.js"
    topWindow.focus();
    topWindow.displayTopic(topic);
  } else {
//@line 61 "/g/mozilla/toolkit/components/help/content/contextHelp.js"
    const params = Components.classes["@mozilla.org/embedcomp/dialogparam;1"]
                             .createInstance(Components.interfaces.nsIDialogParamBlock);
    params.SetNumberStrings(2);
    params.SetString(0, helpFileURI);
    params.SetString(1, topic);
    const ww = Components.classes["@mozilla.org/embedcomp/window-watcher;1"]
                         .getService(Components.interfaces.nsIWindowWatcher);
    ww.openWindow(null, "chrome://help/content/help.xul", "_blank", "chrome,all,alwaysRaised,dialog=no", params);
  }
}

//@line 73 "/g/mozilla/toolkit/components/help/content/contextHelp.js"
function setHelpFileURI(rdfURI)
{
  helpFileURI = rdfURI;
}

//@line 79 "/g/mozilla/toolkit/components/help/content/contextHelp.js"
function locateHelpWindow(contentPack) {
    const windowManagerInterface = Components
        .classes['@mozilla.org/appshell/window-mediator;1'].getService()
        .QueryInterface(Components.interfaces.nsIWindowMediator);
    const iterator = windowManagerInterface.getEnumerator("mozilla:help");
    var topWindow = null;
    var aWindow;

//@line 89 "/g/mozilla/toolkit/components/help/content/contextHelp.js"
    while (iterator.hasMoreElements()) {
        aWindow = iterator.getNext();
        if (aWindow.getHelpFileURI() == contentPack) {
            topWindow = aWindow;
        }
    }
    return topWindow;
}
