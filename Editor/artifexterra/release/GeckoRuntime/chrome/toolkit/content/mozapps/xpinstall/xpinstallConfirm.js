//@line 37 "/g/mozilla/toolkit/mozapps/xpinstall/content/xpinstallConfirm.js"

var XPInstallConfirm = 
{ 
  _param: null
};


XPInstallConfirm.init = function ()
{
  var _installCountdown;
  var _installCountdownInterval;
  var _focused;
  var _timeout;

  var bundle = document.getElementById("xpinstallConfirmStrings");
  
  this._param = window.arguments[0].QueryInterface(Components.interfaces.nsIDialogParamBlock);
  if (!this._param)
    close();
  
  this._param.SetInt(0, 1); // The default return value is "Cancel"
  
  var _installCountdownLength = 5;
  try {
    var prefs = Components.classes["@mozilla.org/preferences-service;1"]
                          .getService(Components.interfaces.nsIPrefBranch);
    var delay_in_milliseconds = prefs.getIntPref("security.dialog_enable_delay");
    _installCountdownLength = Math.round(delay_in_milliseconds / 500);
  } catch (ex) { }
  
  var itemList = document.getElementById("itemList");
  
  var numItemsToInstall = this._param.GetInt(1);
  for (var i = 0; i < numItemsToInstall; ++i) {
    var installItem = document.createElement("installitem");
    itemList.appendChild(installItem);

    installItem.name = this._param.GetString(i);
    installItem.url = this._param.GetString(++i);
    var icon = this._param.GetString(++i);
    if (icon != "")
      installItem.icon = icon;
    var cert = this._param.GetString(++i);
    installItem.cert = cert || bundle.getString("Unsigned");
    installItem.signed = cert ? "true" : "false";
  }
  
  var introString = bundle.getString("itemWarnIntroSingle");
  if (numItemsToInstall > 4)
    introString = bundle.getFormattedString("itemWarnIntroMultiple", [numItemsToInstall / 4]);
  if (this._param.objects && this._param.objects.length)
    introString = this._param.objects.queryElementAt(0, Components.interfaces.nsISupportsString).data;
  var textNode = document.createTextNode(introString);
  var introNode = document.getElementById("itemWarningIntro");
  while (introNode.hasChildNodes())
    introNode.removeChild(introNode.firstChild);
  introNode.appendChild(textNode);
  
  var okButton = document.documentElement.getButton("accept");
  okButton.focus();

  function okButtonCountdown() {
    _installCountdown -= 1;

    if (_installCountdown < 1) {
      okButton.label = bundle.getString("installButtonLabel");
      okButton.disabled = false;
      clearInterval(_installCountdownInterval);
    }
    else
      okButton.label = bundle.getFormattedString("installButtonDisabledLabel", [_installCountdown]);
  }

  function myfocus() {
    // Clear the timeout if it exists so only the last one will be used.
    if (_timeout)
      clearTimeout(_timeout);

    // Use setTimeout since the last focus or blur event to fire is the one we
    // want
    _timeout = setTimeout(setWidgetsAfterFocus, 0);
  }

  function setWidgetsAfterFocus() {
    if (_focused)
      return;

    _installCountdown = _installCountdownLength;
    _installCountdownInterval = setInterval(okButtonCountdown, 500);
    okButton.label = bundle.getFormattedString("installButtonDisabledLabel", [_installCountdown]);
    _focused = true;
  }

  function myblur() {
    // Clear the timeout if it exists so only the last one will be used.
    if (_timeout)
      clearTimeout(_timeout);

    // Use setTimeout since the last focus or blur event to fire is the one we
    // want
    _timeout = setTimeout(setWidgetsAfterBlur, 0);
  }

  function setWidgetsAfterBlur() {
    if (!_focused)
      return;

    // Set _installCountdown to the inital value set in setWidgetsAfterFocus
    // plus 1 so when the window is focused there is immediate ui feedback.
    _installCountdown = _installCountdownLength + 1;
    okButton.label = bundle.getFormattedString("installButtonDisabledLabel", [_installCountdown]);
    okButton.disabled = true;
    clearInterval(_installCountdownInterval);
    _focused = false;
  }

  function myUnload() {
    document.removeEventListener("focus", myfocus, true);
    document.removeEventListener("blur", myblur, true);
    window.removeEventListener("unload", myUnload, false);
  }

  if (_installCountdownLength > 0) {
    document.addEventListener("focus", myfocus, true);
    document.addEventListener("blur", myblur, true);
    window.addEventListener("unload", myUnload, false);

    okButton.disabled = true;
    setWidgetsAfterFocus();
  }
  else
    okButton.label = bundle.getString("installButtonLabel");
}

XPInstallConfirm.onOK = function ()
{
  this._param.SetInt(0, 0);
  return true;
}

XPInstallConfirm.onCancel = function ()
{
  this._param.SetInt(0, 1);
  return true;
}
