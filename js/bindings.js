'use strict'

const vapUtil = require('vaporyjs-util')
const vapashjs = require('vapashjs')
const vapHashUtil = require('vapashjs/util')
const vapashcpp = require('bindings')('vapash')

var messages = require('./messages')

var Vapash = module.exports = function (cacheDB) {
  this.dbOpts = {
    valueEncoding: 'json'
  }
  this.cacheDB = cacheDB
  this.cache = false
  this.light = false
}

// vapash_light_new(block_number)
// returns: { block_number: Number, cache: Buffer }
Vapash.prototype.vapash_light_new = vapashcpp.vapash_light_new

// vapash_light_compute(light, header_hash, nonce)
// returns: { mix_hash: Buffer, result: Buffer }
Vapash.prototype.vapash_light_compute = function (light, header_hash, nonce) {
  if (!light || !light.hasOwnProperty('block_number') || !light.hasOwnProperty('cache')) {
    throw new TypeError(messages.LIGHT_OBJ_INVALID)
  }
  return vapashcpp.vapash_light_compute(light.block_number, light.cache, header_hash, nonce)
}

// mkcache(cacheSize, seed)
// returns: arrays of cache lines
Vapash.prototype.mkcache = function (cacheSize, seed) {
  // get new cache from cpp
  this.cache = vapashcpp.vapash_light_new_internal(cacheSize, seed)
  // cache is a single Buffer here! Not an array of cache lines.
  return this.cache
}

// run(val, nonce, fullSize)
// returns: { mix: Buffer, hash: buffer }
Vapash.prototype.run = function (val, nonce, fullSize) {
  // get new cache from cpp
  var ret = vapashcpp.vapash_light_compute_internal(this.cache, fullSize, val, nonce)

  return {
    mix: ret.mix_hash,
    hash: ret.result
  }
}

Vapash.prototype.headerHash = vapashjs.prototype.headerHash

Vapash.prototype.cacheHash = function () {
  return vapUtil.sha3(this.cache)
}

/**
 * Loads the seed and the cache given a block nnumber
 * @method loadEpoc
 * @param number Number
 * @param cb function
 */
Vapash.prototype.loadEpoc = function (number, cb) {
  var self = this
  const epoc = vapHashUtil.getEpoc(number)

  if (this.epoc === epoc) {
    return cb()
  }

  this.epoc = epoc

  // gives the seed the first epoc found
  function findLastSeed (epoc, cb2) {
    if (epoc === 0) {
      return cb2(vapUtil.zeros(32), 0)
    }

    self.cacheDB.get(epoc, self.dbOpts, function (err, data) {
      if (!err) {
        cb2(data.seed, epoc)
      } else {
        findLastSeed(epoc - 1, cb2)
      }
    })
  }

  /* eslint-disable handle-callback-err */
  self.cacheDB.get(epoc, self.dbOpts, function (err, data) {
    if (!data) {
      self.cacheSize = vapHashUtil.getCacheSize(epoc)
      self.fullSize = vapHashUtil.getFullSize(epoc)

      findLastSeed(epoc, function (seed, foundEpoc) {
        self.seed = vapHashUtil.getSeed(seed, foundEpoc, epoc)
        var cache = self.mkcache(self.cacheSize, self.seed)
        // store the generated cache
        self.cacheDB.put(epoc, {
          cacheSize: self.cacheSize,
          fullSize: self.fullSize,
          seed: self.seed,
          cache: cache
        }, self.dbOpts, cb)
      })
    } else {
      // Object.assign(self, data)
      // cache is a single Buffer here! Not an array of cache lines.
      self.cache = data.cache
      self.cacheSize = data.cacheSize
      self.fullSize = data.fullSize
      self.seed = new Buffer(data.seed)
      cb()
    }
  })
  /* eslint-enable handle-callback-err */
}

Vapash.prototype._verifyPOW = vapashjs.prototype._verifyPOW

Vapash.prototype.verifyPOW = vapashjs.prototype.verifyPOW
