'use strict'

const benchmark = require('benchmark')
const Vapashcpp = require('../js/bindings')
const Vapashjs = require('vapashjs')
const powTests = require('vaporyjs-testing').tests.powTests.vapash_tests

var implementations = {
  bindings: new Vapashcpp(),
  vapashjs: new Vapashjs()
}

var fixtureIndex = 0
var fixtures = new Array(1)
var getNextFixture = function () {
  var fixture = fixtures[fixtureIndex++]
  if (fixtureIndex === fixtures.length) {
    fixtureIndex = 0
  }

  return fixture
}

var testname = 'first'
for (var i = 0; i < fixtures.length; ++i) {
  var test = powTests[testname]

  var fixture = {}
  fixture.cacheSize = test.cache_size
  fixture.seed = new Buffer(test.seed, 'hex')

  fixture.header_hash = new Buffer(test.header_hash, 'hex')
  fixture.nonce = new Buffer(test.nonce, 'hex')
  fixture.fullSize = test.full_size

  fixtures[i] = fixture
  testname = (testname === 'first') ? 'second' : 'fisrt'
}
console.log('Create ' + fixtures.length + ' fixtures')
console.log('++++++++++++++++++++++++++++++++++++++++++++++++++')

function runSuite (suiteName, testFunctionGenerator) {
  var suite = new benchmark.Suite(suiteName, {
    onStart: function () {
      console.log('Benchmarking: ' + suiteName)
      console.log('--------------------------------------------------')
    },
    onCycle: function (event) {
      console.log(String(event.target))
    },
    onError: function (event) {
      console.error(event.target.error)
    },
    onComplete: function () {
      console.log('==================================================')
    }
  })

  Object.keys(implementations).forEach(function (name) {
    suite.add(name, testFunctionGenerator(implementations[name]), {
      onStart: function () {
        fixtureIndex = 0
      },
      onCycle: function () {
        fixtureIndex = 0
      }
    })
  })

  suite.run()
}

runSuite('mkcache', function (vapash) {
  return function () {
    var fixture = getNextFixture()
    vapash.mkcache(fixture.cacheSize, fixture.seed)
  }
})

runSuite('run', function (vapash) {
  return function () {
    var fixture = getNextFixture()
    vapash.run(fixture.header_hash, fixture.nonce, fixture.fullSize)
  }
})
