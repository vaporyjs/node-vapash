# node-vapash

[![js-standard-style](https://cdn.rawgit.com/feross/standard/master/badge.svg)](https://github.com/feross/standard)

Native node bindings for [Vapash cpp implementation](https://github.com/vaporyco/vapash).
This library is experimental. **Use at your own risk**.
Currently working on **Linux only**. Tested on node v5.10.1.

For details on this project, please see the Vapory wiki (https://github.com/vaporyco/wiki/wiki/Vapash)
and vaporyjs/vapashjs (https://github.com/vaporyjs/vapashjs).

## Installation

```
$ git clone https://github.com/vaporyjs/node-vapash.git
$ cd node-vapash/
$ git submodule init
$ git submodule update
$ npm install
```

## API

- [`new Vapash([cacheDB])`](#newvapashcachedb)
- [`vapash.verifyPOW(block, cb)`](#vapashverifypowblock-cb)
- [`vapash.mkcache(cacheSize, seed)`](#vapashmkcachecachesize-seed)
- [`vapash.run(val, nonce, fullSize)`](#vapashrunval-nonce-fullsize)
- [`vapash.loadEpoc(number, cb)`](#vapashloadepocnumber-cb)

### `new Vapash([cacheDB])`
Creates a new instance of `Vapash`.

**Parameters**
- `cacheDB` - an instance of a `levelup` DB which is used to store the cache(s).
Need by [`vapash.verifyPOW()`](#vapashverifypowblock-cb) and
[`vapash.loadEpoc()`](#vapashloadepocnumber-cb)

### `vapash.verifyPOW(block, cb)`
Verifies the POW on a block and its uncles.

Note: uses [`vapash.loadEpoc()`](#vapashloadepocnumber-cb) to load cache.

**Parameters**  
- `block` - the [block](https://github.com/vaporyjs/vaporyjs-block) to verify
- `cb` - callback which is given a `Boolean` determining the validity of the block

### `vapash.mkcache(cacheSize, seed)`
Creates a cache.

NOTE: this is automatically done for in
[`vapash.verifyPOW()`](#vapashverifypowblock-cb)
so you do not need to use this function if you are just validating blocks.

**Parameters**
- `cacheSize` - the size of the cache
- `seed` - the seed as a `Buffer`

### `vapash.run(val, nonce, fullSize)`
Runs vapash on a given val/nonce pair.

NOTE: you need to run [`vapash.mkcache()`](#vapashmkcachecachesize-seed)
first before using this function.

**Parameters**
- `val` - header hash as `Buffer`
- `seed` - the seed as a `Buffer`
- `fullSize` - the fullsize of the cache

**Return**
an `Object` containing
- `hash`  - the hash of the value
- `mix` - the mix result

### `vapash.loadEpoc(number, cb)`
Loads block number epoc's cache from DB.

**Parameters**  
- `number` - the [block's](https://github.com/vaporyjs/vaporyjs-block) number
- `cb` - callback called after the epoc was loaded

## Test
`$ npm test`

## Performance (node-vapash vs vapashjs):
```
$ node benchmark/benchmark.js
Create 1 fixtures
++++++++++++++++++++++++++++++++++++++++++++++++++
Benchmarking: mkcache
--------------------------------------------------
bindings x 1.47 ops/sec ±2.31% (8 runs sampled)
vapashjs x 0.17 ops/sec ±13.59% (5 runs sampled)
==================================================
Benchmarking: run
--------------------------------------------------
bindings x 571 ops/sec ±3.52% (79 runs sampled)
vapashjs x 17.82 ops/sec ±1.58% (48 runs sampled)
==================================================
```

## TODO:
- Implement tests for:
  - [`vapash.verifyPOW()`](#vapashverifypowblock-cb)
  - [`vapash.loadEpoc()`](#vapashloadepocnumber-cb)
- Create more fixtures for [`vapash.run()`](#vapashrunval-nonce-fullsize)'s tests

## LICENSE

This library is free and open-source software released under the MIT license.
