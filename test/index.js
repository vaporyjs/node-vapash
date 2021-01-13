'use strict'
/* global describe */

/**
 * @param {Object} vapash
 * @param {string} description
 */
function test (vapash, description) {
  describe(description, function () {
    require('./vapash')(vapash)
  })
}

test(require('../js/bindings'), 'vapash bindings')
