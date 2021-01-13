#ifndef _VAPASH_NODE_
#define _VAPASH_NODE_

#include <node.h>
#include <nan.h>

NAN_METHOD(vapash_light_new);
NAN_METHOD(vapash_light_compute);
NAN_METHOD(vapash_light_new_internal);
NAN_METHOD(vapash_light_compute_internal);

#endif
