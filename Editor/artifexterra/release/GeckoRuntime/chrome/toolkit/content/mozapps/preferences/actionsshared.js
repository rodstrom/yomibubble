//@line 38 "/g/mozilla/toolkit/mozapps/preferences/actionsshared.js"

const FILEACTION_SAVE_TO_DISK     = 1;
const FILEACTION_OPEN_INTERNALLY  = 2;
const FILEACTION_OPEN_DEFAULT     = 3;
const FILEACTION_OPEN_CUSTOM      = 4;
const FILEACTION_OPEN_PLUGIN      = 5;
function FileAction ()
{
}
FileAction.prototype = {
  type        : "",
  extension   : "",
  hasExtension: true,
  editable    : true,
  smallIcon   : "",
  bigIcon     : "",
  typeName    : "",
  action      : "",
  mimeInfo    : null,
  customHandler       : "",
  handleMode          : false,
  pluginAvailable     : false,
  pluginEnabled       : false,
  handledOnlyByPlugin : false
};


