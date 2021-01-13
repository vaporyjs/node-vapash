#include <stdint.h>
#include <endian.h>
#include <node.h>
#include <nan.h>

#include "util.h"
#include "messages.h"
#include "libvapash/vapash.h"
// hack to avoid conflict between 'node.h' namespace and
// 'node' declared inside internal.h
#define node node_vap
#include "libvapash/internal.h"
#undef node
#define node node

// vapash_light_new(block_number)
// returns: { block_number: Number, cache: Buffer }
NAN_METHOD(vapash_light_new) {
  Nan::HandleScope scope;

  // get block number argument
  v8::Local<v8::Object> block_number_v8 = info[0].As<v8::Object>();
  CHECK_TYPE_NUMBER(block_number_v8, BLOCKNUM_TYPE_INVALID);
  // node -> C
  const uint64_t block_number = block_number_v8->IntegerValue();

  // get new vapash_light handler
  vapash_light_t light = vapash_light_new(block_number);
  if (light == NULL) {
    return Nan::ThrowError(LIGHTNEW_NOMEM);
  }

  // C -> node
  v8::Local<v8::Object> obj = Nan::New<v8::Object>();
  obj->Set(Nan::New<v8::String>("block_number").ToLocalChecked(),
    Nan::New<v8::Number>(block_number));
  obj->Set(Nan::New<v8::String>("cache").ToLocalChecked(),
    COPY_BUFFER((const char *)light->cache, light->cache_size));
  info.GetReturnValue().Set(obj);

  // free vapash_light handler
  vapash_light_delete(light);
}

// vapash_light_compute(block_number, cache, header_hash, nonce)
// returns: { mix_hash: Buffer, result: Buffer }
NAN_METHOD(vapash_light_compute) {
  Nan::HandleScope scope;

  struct vapash_light light;

  // get block number argument
  v8::Local<v8::Object> block_number_v8 = info[0].As<v8::Object>();
  CHECK_TYPE_NUMBER(block_number_v8, BLOCKNUM_TYPE_INVALID);
  // node -> C
  light.block_number = block_number_v8->IntegerValue();

  // get cache argument
  v8::Local<v8::Object> cache_v8 = info[1].As<v8::Object>();
  CHECK_TYPE_BUFFER(cache_v8, CACHE_TYPE_INVALID);
  // node -> C
  light.cache = (void *) node::Buffer::Data(cache_v8);
  light.cache_size = node::Buffer::Length(cache_v8);

  // get header hash
  v8::Local<v8::Object> header_hash_v8 = info[2].As<v8::Object>();
  CHECK_TYPE_BUFFER(header_hash_v8, HEADERHASH_TYPE_INVALID);
  CHECK_BUFFER_LENGTH(header_hash_v8, 32, HEADERHASH_LENGTH_INVALID);
  // node -> C
  vapash_h256_t *header_hash = (vapash_h256_t *) node::Buffer::Data(header_hash_v8);

  // get nonce argument
  v8::Local<v8::Object> nonce_v8 = info[3].As<v8::Object>();
  CHECK_TYPE_BUFFER(nonce_v8, NONCE_TYPE_INVALID);
  CHECK_BUFFER_LENGTH(nonce_v8, 8, NONCE_LENGTH_INVALID);
  // node -> C
  const uint64_t nonce = be64toh(*((uint64_t *) node::Buffer::Data(nonce_v8)));

  // calculate light client data
  vapash_return_value_t ret = vapash_light_compute(&light, *header_hash, nonce);
  if (!ret.success) {
    return Nan::ThrowError(LIGHTCOMPUTE_ERROR);
  }

  // C -> node
  v8::Local<v8::Object> obj = Nan::New<v8::Object>();
  obj->Set(Nan::New<v8::String>("mix_hash").ToLocalChecked(),
    COPY_BUFFER((const char *)&ret.mix_hash, sizeof(vapash_h256_t)));
  obj->Set(Nan::New<v8::String>("result").ToLocalChecked(),
    COPY_BUFFER((const char *)&ret.result, sizeof(vapash_h256_t)));
  info.GetReturnValue().Set(obj);
}

// vapash_light_new_internal(cache_size, seed)
// returns: Buffer
NAN_METHOD(vapash_light_new_internal) {
  Nan::HandleScope scope;

  // get cache size
  v8::Local<v8::Object> cache_size_v8 = info[0].As<v8::Object>();
  CHECK_TYPE_NUMBER(cache_size_v8, CACHESIZE_TYPE_INVALID);
  // node -> C
  const uint64_t cache_size = cache_size_v8->IntegerValue();

  // get seed
  v8::Local<v8::Object> seed_v8 = info[1].As<v8::Object>();
  CHECK_TYPE_BUFFER(seed_v8, SEED_TYPE_INVALID);
  CHECK_BUFFER_LENGTH(seed_v8, 32, SEED_LENGTH_INVALID);
  // node -> C
  vapash_h256_t *seed = (vapash_h256_t *) node::Buffer::Data(seed_v8);

  // Frustrated attempt on optimizing buffer creation to avoid copying =/
  // vapash_compute_cache_nodes() is static and can't be used inside _light_new_internal()
  // left here to aid future attempts {
  //
  // // get a buffer for cache
  // v8::Local<v8::Object> buf = Nan::NewBuffer(cache_size).ToLocalChecked();
  // // local light structure
  // struct vapash_light light;
  //
  // // get new cache
  // _light_new_internal(node::Buffer::Data(buf), &light, cache_size, seed);
  //
  // // C -> node
  // info.GetReturnValue().Set(buf);
  //
  // }

  // let there be light
  vapash_light_t light = vapash_light_new_internal(cache_size, seed);
  if (light == NULL) {
    return Nan::ThrowError(LIGHTNEW_NOMEM);
  }

  // C -> node
  info.GetReturnValue().Set(COPY_BUFFER((const char *)light->cache, light->cache_size));

  // free vapash_light handler
  vapash_light_delete(light);
}

// vapash_light_compute_internal(cache, full_size, header_hash, nonce)
// returns: { mix_hash: Buffer, result: Buffer }
NAN_METHOD(vapash_light_compute_internal) {
  Nan::HandleScope scope;

  struct vapash_light light;

  // get cache argument
  v8::Local<v8::Object> cache_v8 = info[0].As<v8::Object>();
  CHECK_TYPE_BUFFER(cache_v8, CACHE_TYPE_INVALID);
  // node -> C
  light.cache = (void *) node::Buffer::Data(cache_v8);
  light.cache_size = node::Buffer::Length(cache_v8);

  // get full_size argument
  v8::Local<v8::Object> full_size_v8 = info[1].As<v8::Object>();
  CHECK_TYPE_NUMBER(full_size_v8, FULLSIZE_TYPE_INVALID);
  // node -> C
  uint64_t full_size = full_size_v8->IntegerValue();

  // get header hash
  v8::Local<v8::Object> header_hash_v8 = info[2].As<v8::Object>();
  CHECK_TYPE_BUFFER(header_hash_v8, HEADERHASH_TYPE_INVALID);
  CHECK_BUFFER_LENGTH(header_hash_v8, 32, HEADERHASH_LENGTH_INVALID);
  // node -> C
  vapash_h256_t *header_hash = (vapash_h256_t *) node::Buffer::Data(header_hash_v8);

  // get nonce argument
  v8::Local<v8::Object> nonce_v8 = info[3].As<v8::Object>();
  CHECK_TYPE_BUFFER(nonce_v8, NONCE_TYPE_INVALID);
  CHECK_BUFFER_LENGTH(nonce_v8, 8, NONCE_LENGTH_INVALID);
  // node -> C
  const uint64_t nonce = be64toh(*((uint64_t *) node::Buffer::Data(nonce_v8)));

  vapash_return_value_t ret = vapash_light_compute_internal(
    &light, full_size, *header_hash, nonce);
  if (!ret.success) {
    return Nan::ThrowError(LIGHTCOMPUTE_ERROR);
  }

  // C -> node
  v8::Local<v8::Object> obj = Nan::New<v8::Object>();
  obj->Set(Nan::New<v8::String>("mix_hash").ToLocalChecked(),
    COPY_BUFFER((const char *)&ret.mix_hash, sizeof(vapash_h256_t)));
  obj->Set(Nan::New<v8::String>("result").ToLocalChecked(),
    COPY_BUFFER((const char *)&ret.result, sizeof(vapash_h256_t)));
  info.GetReturnValue().Set(obj);
}

NAN_MODULE_INIT(Init) {
  Nan::Export(target, "vapash_light_new", vapash_light_new);
  Nan::Export(target, "vapash_light_compute", vapash_light_compute);
  Nan::Export(target, "vapash_light_new_internal", vapash_light_new_internal);
  Nan::Export(target, "vapash_light_compute_internal", vapash_light_compute_internal);
}

NODE_MODULE(vapash, Init);
