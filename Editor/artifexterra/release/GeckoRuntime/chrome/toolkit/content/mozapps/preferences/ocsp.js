//@line 38 "/g/mozilla/toolkit/mozapps/preferences/ocsp.js"

var gOCSPDialog = {
  _certDB         : null,
  _OCSPResponders : null,

  init: function ()
  {
    this._certDB = Components.classes["@mozilla.org/security/x509certdb;1"]
                             .getService(Components.interfaces.nsIX509CertDB);
    this._OCSPResponders = this._certDB.getOCSPResponders();

    var signingCA = document.getElementById("signingCA");
    const nsIOCSPResponder = Components.interfaces.nsIOCSPResponder;
    for (var i = 0; i < this._OCSPResponders.length; ++i) {
      var ocspEntry = this._OCSPResponders.queryElementAt(i, nsIOCSPResponder);
      var menuitem = document.createElement("menuitem");
      menuitem.setAttribute("value", ocspEntry.responseSigner);
      menuitem.setAttribute("label", ocspEntry.responseSigner);
      signingCA.firstChild.appendChild(menuitem);
    }
    
    var signingCAPref = document.getElementById("security.OCSP.signingCA");
    if (!signingCAPref.hasUserValue)
      signingCA.selectedIndex = 0;
    else {
      // We need to initialize manually since auto-initialization is often 
      // called prior to menulist population above.
      signingCA.value = signingCAPref.value;
    }
    this.chooseServiceURL();
  },
  
  _updateUI: function ()
  {
    var signingCA = document.getElementById("security.OCSP.signingCA");
    var serviceURL = document.getElementById("security.OCSP.URL");
    var securityOCSPEnabled = document.getElementById("security.OCSP.enabled");

    var OCSPEnabled = parseInt(securityOCSPEnabled.value);
    signingCA.disabled = serviceURL.disabled = OCSPEnabled == 0 || OCSPEnabled == 1;
    return undefined;
  },
  
  chooseServiceURL: function ()
  {
    var signingCA = document.getElementById("signingCA");
    var serviceURL = document.getElementById("serviceURL");
    var CA = signingCA.value;
    
    const nsIOCSPResponder = Components.interfaces.nsIOCSPResponder;
    for (var i = 0; i < this._OCSPResponders.length; ++i) {
      var ocspEntry = this._OCSPResponders.queryElementAt(i, nsIOCSPResponder);
      if (CA == ocspEntry.responseSigner) {
        serviceURL.value = ocspEntry.serviceURL;
        break;
      }
    }
  }
};
