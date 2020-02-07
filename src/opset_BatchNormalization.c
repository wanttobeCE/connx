#include <math.h>
#include <connx/connx.h>

static bool BatchNormalization_resolve(uintptr_t* stack) {
	connx_Tensor* Y = (void*)stack[1];
	connx_Tensor* X = (void*)stack[2];
	connx_Tensor* scale  = (void*)stack[3];
	connx_Tensor* B = (void*)stack[4];
	connx_Tensor* mean = (void*)stack[5];
	connx_Tensor* var = (void*)stack[6];

	if(!connx_Tensor_isShapeEquals(X, Y)) {
		connx_exception("X and Y's shape is differ");
		return false;
	}

	if(X->dimension < 3) {
		connx_exception("X and Y's dimension must be greater than 3 but %u", X->dimension);
		return false;
	}

	if(scale->dimension != 1) {
		connx_exception("scale must be a 1 dimensional tensor but %u", scale->dimension);
		return false;
	}

	if(scale->lengths[0] != X->lengths[1]) {
		connx_exception("scale's length must be equals to X or Y's channel count %u but %u", X->lengths[1], scale->lengths[0]);
		return false;
	}

	if(scale->elemType != Y->elemType) {
		connx_exception("scale's type is differ from X or Y's type: %u", scale->elemType);
		return false;
	}

	if(B->dimension != 1) {
		connx_exception("B must be a 1 dimensional tensor but %u", B->dimension);
		return false;
	}

	if(B->lengths[0] != X->lengths[1]) {
		connx_exception("B's length must be equals to X or Y's channel count %u but %u", X->lengths[1], B->lengths[0]);
		return false;
	}

	if(B->elemType != Y->elemType) {
		connx_exception("B's type is differ from X or Y's type: %u", B->elemType);
		return false;
	}

	if(mean->dimension != 1) {
		connx_exception("mean must be a 1 dimensional tensor but %u", mean->dimension);
		return false;
	}

	if(mean->lengths[0] != X->lengths[1]) {
		connx_exception("mean's length must be equals to X or Y's channel count %u but %u", X->lengths[1], mean->lengths[0]);
		return false;
	}

	if(mean->elemType != Y->elemType) {
		connx_exception("mean's type is differ from X or Y's type: %u", mean->elemType);
		return false;
	}

	if(var->dimension != 1) {
		connx_exception("var must be a 1 dimensional tensor but %u", var->dimension);
		return false;
	}

	if(var->lengths[0] != X->lengths[1]) {
		connx_exception("var's length must be equals to X or Y's channel count %u but %u", X->lengths[1], var->lengths[0]);
		return false;
	}

	if(var->elemType != Y->elemType) {
		connx_exception("var's type is differ from X or Y's type: %u", var->elemType);
		return false;
	}

	return true;
}

static bool BatchNormalization_exec(uintptr_t* stack) {
	connx_Tensor* Y = (void*)stack[1];
	connx_Tensor* X = (void*)stack[2];
	connx_Tensor* scale  = (void*)stack[3];
	connx_Tensor* B = (void*)stack[4];
	connx_Tensor* mean = (void*)stack[5];
	connx_Tensor* var = (void*)stack[6];

	uint32_t batch_count = Y->lengths[0];
	uint32_t channel_count = Y->lengths[1];
	uint32_t unit_count = 1;
	for(uint32_t i = 2; i < Y->dimension; i++)
		unit_count *= Y->lengths[i];

	switch(Y->elemType) {
		case connx_DataType_FLOAT16:
		case connx_DataType_FLOAT32:
			{
				float* Y_base = (float*)Y->base;
				float* X_base = (float*)X->base;

				float* scales = (float*)scale->base;
				float* Bs = (float*)B->base;
				float* means = (float*)mean->base;
				float* vars = (float*)var->base;

				float* epsilon = (float*)stack[7];

				for(uint32_t b = 0; b < batch_count; b++) {
					for(uint32_t c = 0; c < channel_count; c++) {
						float scale_value = scales[c];
						float B_value = Bs[c];
						float mean_value = means[c];
						float sqrt_value = sqrtf(vars[c] + *epsilon);

						for(uint32_t u = 0; u < unit_count; u++) {
							*Y_base++ = scale_value * (*X_base++ - mean_value) / sqrt_value + B_value;
						}
					}
				}
			}
			break;
		case connx_DataType_FLOAT64:
			{
				double* Y_base = (double*)Y->base;
				double* X_base = (double*)X->base;

				double* scales = (double*)scale->base;
				double* Bs = (double*)B->base;
				double* means = (double*)mean->base;
				double* vars = (double*)var->base;

				double* epsilon = (double*)stack[7];

				for(uint32_t b = 0; b < batch_count; b++) {
					for(uint32_t c = 0; c < channel_count; c++) {
						double scale_value = scales[c];
						double B_value = Bs[c];
						double mean_value = means[c];
						double sqrt_value = sqrt(vars[c] + *epsilon);

						for(uint32_t u = 0; u < unit_count; u++) {
							*Y_base++ = scale_value * (*X_base++ - mean_value) / sqrt_value + B_value;
						}
					}
				}
			}
			break;
		default:
			return false;
	}

	return true;
}

bool connx_opset_BatchNormalization_init() {
	connx_Operator_add("BatchNormalization", 1, 5, 2, BatchNormalization_resolve, BatchNormalization_exec,
		connx_DataType_TENSOR_FLOAT,	// Y
		connx_DataType_TENSOR_FLOAT,	// X
		connx_DataType_TENSOR_FLOAT,	// scale
		connx_DataType_TENSOR_FLOAT,	// B
		connx_DataType_TENSOR_FLOAT,	// mean
		connx_DataType_TENSOR_FLOAT,	// var
		"epsilon", connx_DataType_FLOAT32, 0.00005,
		"momentum", connx_DataType_FLOAT32, 0.9);

	return true;
}