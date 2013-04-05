//@line 39 "/g/mozilla/toolkit/content/fontpackage.js"

var gLangCode;

function onLoad()
{
  var size = document.getElementById("size");
  var downloadButton = document.getElementById("downloadButton");
  var install = document.getElementById("install");
  var fontPackageBundle = document.getElementById("fontPackageBundle");

  // test if win2k (win nt 5.0) or winxp (win nt 5.1)
  if (navigator.userAgent.toLowerCase().indexOf("windows nt 5") != -1) 
  {
    downloadButton.setAttribute("hidden", "true");
    size.setAttribute("hidden", "true");

    // if no download button
    // set title to "Install Font"
    // and set cancel button to "OK"
    document.title = fontPackageBundle.getString("windowTitleNoDownload");
    var cancelButton = document.getElementById("cancelButton");
    cancelButton.setAttribute("label", fontPackageBundle.getString("cancelButtonNoDownload"));
  } 
  else 
  {
    install.setAttribute("hidden", "true");
  }

  // argument is a lang code of the form xx or xx-yy
  gLangCode = window.arguments[0];

  var titleString = fontPackageBundle.getString("name_" + gLangCode);
  var languageTitle = document.getElementById("languageTitle");
  languageTitle.setAttribute("value", titleString);
  
  var sizeString = fontPackageBundle.getString("size_" + gLangCode);
  var sizeSpecification = document.getElementById("sizeSpecification");
  sizeSpecification.setAttribute("value", sizeString);
}

function download()
{ 
  window.open("http://www.mozilla.org/projects/intl/fonts/win/redirect/package_" + gLangCode + ".html");
}

