//@line 44 "/g/mozilla/toolkit/content/debug.js"

var gTraceOnAssert = true;

/**
 * This function provides a simple assertion function for JavaScript.
 * If the condition is true, this function will do nothing.  If the
 * condition is false, then the message will be printed to the console
 * and an alert will appear showing a stack trace, so that the (alpha
 * or nightly) user can file a bug containing it.  For future enhancements, 
 * see bugs 330077 and 330078.
 *
 * To suppress the dialogs, you can run with the environment variable
 * XUL_ASSERT_PROMPT set to 0 (if unset, this defaults to 1).
 *
 * @param condition represents the condition that we're asserting to be
 *                  true when we call this function--should be
 *                  something that can be evaluated as a boolean.
 * @param message   a string to be displayed upon failure of the assertion
 */

function NS_ASSERT(condition, message) {
  if (condition)
    return;

  var assertionText = "ASSERT: " + message + "\n";

//@line 72 "/g/mozilla/toolkit/content/debug.js"
  Components.util.reportError(assertionText);
  return;
//@line 108 "/g/mozilla/toolkit/content/debug.js"
}
