//@line 40 "/g/mozilla/toolkit/content/viewZoomOverlay.js"

/** Document Zoom Management Code
 *
 * To use this, you'll need to have a getMarkupDocumentViewer() function which returns a
 * nsIMarkupDocumentViewer.
 *
 **/


function ZoomManager() {
}

ZoomManager.prototype = {
  instance : null,

  getInstance : function() {
    if (!ZoomManager.prototype.instance)
      ZoomManager.prototype.instance = new ZoomManager();

    return ZoomManager.prototype.instance;
  },

  MIN : 1,
  MAX : 2000,
  factorOther : 300,
  factorAnchor : 300,
    zoomFactors: [50, 75, 90, 100, 120, 150, 200],
  steps : 0,

  get textZoom() {
    var currentZoom;
    try {
      currentZoom = Math.round(getMarkupDocumentViewer().textZoom * 100);
      if (this.indexOf(currentZoom) == -1) {
        if (currentZoom != this.factorOther) {
          this.factorOther = currentZoom;
          this.factorAnchor = this.factorOther;
        }
      }
    } catch (e) {
      currentZoom = 100;
    }
    return currentZoom;
  },

  set textZoom(aZoom) {
    if (aZoom < this.MIN || aZoom > this.MAX)
      throw Components.results.NS_ERROR_INVALID_ARG;

    getMarkupDocumentViewer().textZoom = aZoom / 100;
  },

  enlarge : function() {
    this.jump(1);
  },

  reduce : function() {
    this.jump(-1);
  },
  reset : function() {
    this.textZoom = 100;
  },
  indexOf : function(aZoom) {
    var index = -1;
    if (this.isZoomInRange(aZoom)) {
      index = this.zoomFactors.length - 1;
      while (index >= 0 && this.zoomFactors[index] != aZoom)
        --index;
    }

    return index;
  },

  /***** internal helper functions below here *****/

  isZoomInRange : function(aZoom) {
    return (aZoom >= this.zoomFactors[0] && aZoom <= this.zoomFactors[this.zoomFactors.length - 1]);
  },

  jump : function(aDirection) {
    if (aDirection != -1 && aDirection != 1)
      throw Components.results.NS_ERROR_INVALID_ARG;

    var currentZoom = this.textZoom;
    var insertIndex = -1;
    const stepFactor = 1.5;

    // temporarily add factorOther to list
    if (this.isZoomInRange(this.factorOther)) {
      insertIndex = 0;
      while (this.zoomFactors[insertIndex] < this.factorOther)
        ++insertIndex;

      if (this.zoomFactors[insertIndex] != this.factorOther)
        this.zoomFactors.splice(insertIndex, 0, this.factorOther);
    }

    var factor;
    var done = false;

    if (this.isZoomInRange(currentZoom)) {
      var index = this.indexOf(currentZoom);
      if (aDirection == -1 && index == 0 ||
          aDirection ==  1 && index == this.zoomFactors.length - 1) {
        this.steps = 0;
        this.factorAnchor = this.zoomFactors[index];
      } else {
        factor = this.zoomFactors[index + aDirection];
        done = true;
      }
    }

    if (!done) {
      this.steps += aDirection;
      factor = this.factorAnchor * Math.pow(stepFactor, this.steps);
      if (factor < this.MIN || factor > this.MAX) {
        this.steps -= aDirection;
        factor = this.factorAnchor * Math.pow(stepFactor, this.steps);
      }
      factor = Math.round(factor);
      if (this.isZoomInRange(factor))
        factor = this.snap(factor);
      else
        this.factorOther = factor;
    }

    if (insertIndex != -1)
      this.zoomFactors.splice(insertIndex, 1);

    this.textZoom = factor;
  },

  snap : function(aZoom) {
    if (this.isZoomInRange(aZoom)) {
      var level = 0;
      while (this.zoomFactors[level + 1] < aZoom)
        ++level;

      // if aZoom closer to [level + 1] than [level], snap to [level + 1]
      if ((this.zoomFactors[level + 1] - aZoom) < (aZoom - this.zoomFactors[level]))
        ++level;

      aZoom = this.zoomFactors[level];
    }

    return aZoom;
  }
}


