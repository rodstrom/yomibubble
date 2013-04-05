//@line 38 "/g/mozilla/toolkit/components/console/content/console.js"

var gConsole, gConsoleBundle, gTextBoxEval;

/* :::::::: Console Initialization ::::::::::::::: */

window.onload = function()
{
  gConsole = document.getElementById("ConsoleBox");
  gConsoleBundle = document.getElementById("ConsoleBundle");
  gTextBoxEval = document.getElementById("TextboxEval")  
  
  updateSortCommand(gConsole.sortOrder);
  updateModeCommand(gConsole.mode);

  var iframe = document.getElementById("Evaluator");
  iframe.addEventListener("load", displayResult, true);
}

/* :::::::: Console UI Functions ::::::::::::::: */

function changeMode(aMode)
{
  switch (aMode) {
    case "Errors":
    case "Warnings":
    case "Messages":
      gConsole.mode = aMode;
      break;
    case "All":
      gConsole.mode = null;
  }
  
  document.persist("ConsoleBox", "mode");
}

function clearConsole()
{
  gConsole.clear();
}

function changeSortOrder(aOrder)
{
  updateSortCommand(gConsole.sortOrder = aOrder);
}

function updateSortCommand(aOrder)
{
  var orderString = aOrder == 'reverse' ? "Descend" : "Ascend";
  var bc = document.getElementById("Console:sort"+orderString);
  bc.setAttribute("checked", true);  

  orderString = aOrder == 'reverse' ? "Ascend" : "Descend";
  bc = document.getElementById("Console:sort"+orderString);
  bc.setAttribute("checked", false);
}

function updateModeCommand(aMode)
{
  var bc = document.getElementById("Console:mode" + aMode);
  bc.setAttribute("checked", true);
}

function copyItemToClipboard()
{
  gConsole.copySelectedItem();
}

function isItemSelected()
{
  return gConsole.selectedItem != null;
}

function updateCopyMenu()
{
  goSetCommandEnabled("cmd_copy", isItemSelected())
}

function onEvalKeyPress(aEvent)
{
  if (aEvent.keyCode == 13)
    evaluateTypein();
}

function evaluateTypein()
{
  var code = gTextBoxEval.value;
  var iframe = document.getElementById("Evaluator");
  iframe.setAttribute("src", "javascript: " + code);
}

function displayResult()
{
  var resultRange = Evaluator.document.createRange();
  resultRange.selectNode(Evaluator.document.documentElement);
  var result = resultRange.toString();
  if (result)
    gConsole.mCService.logStringMessage(result);
    // or could use appendMessage which doesn't persist
  var iframe = document.getElementById("Evaluator");
  iframe.setAttribute("src", "chrome://global/content/blank.html");
}

// XXX DEBUG
function debug(aText)
{
  var csClass = Components.classes['@mozilla.org/consoleservice;1'];
  var cs = csClass.getService(Components.interfaces.nsIConsoleService);
  cs.logStringMessage(aText);
}
