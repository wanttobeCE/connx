#ifndef __CONNX_TENSOR_H__
#define __CONNX_TENSOR_H__

#include "types.h"
#include "hal.h"

// The number of the enumeration follow ONNX's TensorProto.DataType code
typedef enum _connx_DataType {
    UNDEFINED   = 0,
    UINT8       = 2,
    INT8        = 3,
    UINT16      = 4,
    INT16       = 5,
    UINT32      = 12,
    INT32       = 6,
    UINT64      = 13,
    INT64       = 7,
    FLOAT16     = 10,
    FLOAT32     = 1,
    FLOAT64     = 11,
    STRING      = 8,
    BOOL        = 9,
    COMPLEX64   = 14,
    COMPLEX128  = 15,
} connx_DataType;

uint32_t connx_DataType_size(connx_DataType dtype);

// Iterator
/**
 * iterator - 1 + ndim * 4
 */
void connx_Iterator_init(int32_t* iterator, int32_t ndim, int32_t* start, int32_t* stop, int32_t* step);
bool connx_Iterator_next(int32_t* iterator);
int32_t connx_Iterator_ndim(int32_t* iterator);
int32_t* connx_Iterator_start(int32_t* iterator);
int32_t* connx_Iterator_stop(int32_t* iterator);
int32_t* connx_Iterator_step(int32_t* iterator);
int32_t* connx_Iterator_index(int32_t* iterator);
int32_t connx_Iterator_offset(int32_t* iterator, int32_t* shape);
void connx_Iterator_dump(int32_t* iterator);

// tensor structure follow Numpy's ndarray
typedef struct _connx_Tensor {
    connx_DataType  dtype;      // data type
    int32_t         ndim;       // Number of dimensions
    int32_t*        shape;      // Shape array
    void*           buffer;     // Data buffer
    uint32_t        size;       // size of buffer
    uint32_t        offset;     // offset in buffer
    int32_t         ref_count;  // Reference count
    connx_Lock      lock;       // Reference lock
} connx_Tensor;

int32_t connx_Iterator_size(connx_Tensor* tensor);

connx_Tensor* connx_Tensor_alloc(connx_DataType dtype, int32_t ndim, int32_t* shape);
connx_Tensor* connx_Tensor_alloc_like(connx_Tensor* tensor);
void connx_Tensor_ref(connx_Tensor* tensor);
void connx_Tensor_unref(connx_Tensor* tensor);

int connx_Tensor_get(connx_Tensor* tensor, int32_t* idx, void* data);
int connx_Tensor_set(connx_Tensor* tensor, int32_t* idx, void* data);
void connx_Tensor_dump(connx_Tensor* tensor);

int connx_Tensor_add(connx_Tensor* y, connx_Tensor* a, connx_Tensor* b);
int connx_Tensor_sub(connx_Tensor* y, connx_Tensor* a, connx_Tensor* b);
int connx_Tensor_mul(connx_Tensor* y, connx_Tensor* a, connx_Tensor* b);
int connx_Tensor_matmul(connx_Tensor* y, connx_Tensor* a, connx_Tensor* b);
int connx_Tensor_and(connx_Tensor* y, connx_Tensor* x);
int connx_Tensor_abs(connx_Tensor* y, connx_Tensor* x);
int connx_Tensor_acos(connx_Tensor* y, connx_Tensor* x);
int connx_Tensor_acosh(connx_Tensor* y, connx_Tensor* x);
int connx_Tensor_argmax(int32_t ndim, int32_t* idx, connx_Tensor* x);
int connx_Tensor_argmin(int32_t ndim, int32_t* idx, connx_Tensor* x);

#endif /* __CONNX_TENSOR_H__ */