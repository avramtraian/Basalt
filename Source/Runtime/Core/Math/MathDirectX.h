// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

/**
 * This file contains the definitions and implementations of the Basalt vector math library,
 * using the DirectXMath API. It is the default implementation on Windows.
 */

#if BT_PLATFORM_WINDOWS
    #include <DirectXMath.h>
#else
    #error Trying to include the DirectXMath header files!
#endif // BT_PLATFORM_WINDOWS

namespace Basalt
{
namespace Math
{

/**
 * Four-component vector register composed of floats.
 */
using VectorRegister = DirectX::XMVECTOR;

/**
 * Creates a vector register based on 4 floats.
 * 
 * @param x 1st float component.
 * @param y 2nd float component.
 * @param z 3rd float component.
 * @param w 4th float component.
 * 
 * @return The vector register.
 */
FORCEINLINE VectorRegister MakeVectorRegister(float x, float y, float z, float w)
{
    return DirectX::XMVectorSet(x, y, z, w);
}

/**
 * Multiplies two 4x4 matrices.
 * 
 * @param destinationMatrix Pointer to where the result of the multiplication will be stored.
 * @param matrixA Pointer to the first matrix to multiply.
 * @param matrixA Pointer to the second matrix to multiply.
 */
FORCEINLINE void VectorMatrixMultiply(Matrix4f* destinationMatrix, const Matrix4f* matrixA, const Matrix4f* matrixB)
{
    using namespace DirectX;
    XMMATRIX matA = XMLoadFloat4x4((const XMFLOAT4X4*)matrixA);
    XMMATRIX matB = XMLoadFloat4x4((const XMFLOAT4X4*)matrixB);
    XMMATRIX result = XMMatrixMultiply(matA, matB);
    XMStoreFloat4x4((XMFLOAT4X4*)destinationMatrix, result);
}

/**
 * Multiplies a 4-component vector with a 4x4 matrix.
 * This function transforms the point determined by the vector with the given transformation matrix.
 * 
 * @param destinationVector Pointer to where the result of the multiplication (or transformation) will be stored.
 * @param vector Pointer to the 4-component vector.
 * @param matrix Pointer to the 4x4 matrix.
 */
FORCEINLINE void VectorMatrixTransformPoint(Vector4f* destinationVector, const Vector4f* vector, const Matrix4f* matrix)
{
    using namespace DirectX;
    XMVECTOR vec = XMLoadFloat4((const XMFLOAT4*)vector);
    XMMATRIX mat = XMLoadFloat4x4((const XMFLOAT4X4*)matrix);
    XMVECTOR result = XMVector4Transform(vec, mat);
    XMStoreFloat4((XMFLOAT4*)destinationVector, result);
}

/**
 * Calculates the transpose of a 4x4 matrix.
 * 
 * @param destinationMatrix Pointer to where the result will be stored.
 * @param sourceMatrix Pointer to the matrix to calculate its transpose.
 */
FORCEINLINE void VectorMatrixTranspose(Matrix4f* destinationMatrix, const Matrix4f* sourceMatrix)
{
    using namespace DirectX;
    XMMATRIX source = XMLoadFloat4x4((const XMFLOAT4X4*)sourceMatrix);
    XMMATRIX result = XMMatrixTranspose(source);
    XMStoreFloat4x4((XMFLOAT4X4*)destinationMatrix, result);
}

/**
 * Calculates the inverse of a 4x4 matrix.
 * 
 * @param destinationMatrix Pointer to where the result will be stored.
 * @param sourceMatrix Pointer to the matrix to calculate its inverse.
 */
FORCEINLINE void VectorMatrixInverse(Matrix4f* destinationMatrix, const Matrix4f* sourceMatrix)
{
    using namespace DirectX;
    XMMATRIX source = XMLoadFloat4x4((const XMFLOAT4X4*)sourceMatrix);
    XMMATRIX result = XMMatrixInverse(nullptr, source);
    XMStoreFloat4x4((XMFLOAT4X4*)destinationMatrix, result);
}

/**
 * Creates a rotation matrix along the X-axis.
 * 
 * @param destinationMatrix Pointer to where the result will be stored.
 * @param angleX The angle in radians to rotate with.
 */
FORCEINLINE void VectorMatrixRotateX(Matrix4f* destinationMatrix, float angleX)
{
    using namespace DirectX;
    XMMATRIX result = XMMatrixRotationX(angleX);
    XMStoreFloat4x4((XMFLOAT4X4*)destinationMatrix, result);
}

/**
 * Creates a rotation matrix along the Y-axis.
 *
 * @param destinationMatrix Pointer to where the result will be stored.
 * @param angleX The angle in radians to rotate with.
 */
FORCEINLINE void VectorMatrixRotateY(Matrix4f* destinationMatrix, float angleY)
{
    using namespace DirectX;
    XMMATRIX result = XMMatrixRotationY(angleY);
    XMStoreFloat4x4((XMFLOAT4X4*)destinationMatrix, result);
}

/**
 * Creates a rotation matrix along the Z-axis.
 *
 * @param destinationMatrix Pointer to where the result will be stored.
 * @param angleX The angle in radians to rotate with.
 */
FORCEINLINE void VectorMatrixRotateZ(Matrix4f* destinationMatrix, float angleZ)
{
    using namespace DirectX;
    XMMATRIX result = XMMatrixRotationZ(angleZ);
    XMStoreFloat4x4((XMFLOAT4X4*)destinationMatrix, result);
}

/**
 * Creates a rotation matrix from Euler angles.
 * 
 * @param destinationMatrix Pointer to where the result will be stored.
 * @param roll The angle in radians to rotate along the the roll axis.
 * @param pitch The angle in radians to rotate along the the pitch axis.
 * @param yaw The angle in radians to rotate along the the yaw axis.
 */
FORCEINLINE void VectorMatrixRotateRollPitchYaw(Matrix4f* destinationMatrix, float roll, float pitch, float yaw)
{
    using namespace DirectX;
    XMMATRIX result = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
    XMStoreFloat4x4((XMFLOAT4X4*)destinationMatrix, result);
}

/**
 * Creates a left-handed perspective projection matrix based on a field of view.
 * 
 * @param destinationMatrix Pointer to where the result will be stored.
 * @param fov Top-down field-of-view angle, expressed in radians.
 * @param nearZ The distance to the near clipping plane.
 * @param farZ The distance to the far clipping plane.
 */
FORCEINLINE void VectorMatrixPerspectiveLH(Matrix4f* destinationMatrix, float fov, float aspectRatio, float nearZ, float farZ)
{
    using namespace DirectX;
    XMMATRIX result = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
    XMStoreFloat4x4((XMFLOAT4X4*)destinationMatrix, result);
}

/**
 * Creates a right-handed perspective projection matrix based on a field of view.
 *
 * @param destinationMatrix Pointer to where the result will be stored.
 * @param fov Top-down field-of-view angle, expressed in radians.
 * @param nearZ The distance to the near clipping plane.
 * @param farZ The distance to the far clipping plane.
 */
FORCEINLINE void VectorMatrixPerspectiveRH(Matrix4f* destinationMatrix, float fov, float aspectRatio, float nearZ, float farZ)
{
    using namespace DirectX;
    XMMATRIX result = XMMatrixPerspectiveFovRH(fov, aspectRatio, nearZ, farZ);
    XMStoreFloat4x4((XMFLOAT4X4*)destinationMatrix, result);
}

/**
 * Creates a left-handed orthographic projection matrix.
 *
 * @param destinationMatrix Pointer to where the result will be stored.
 * @param viewWidth The width of the frustum at the near clipping plane.
 * @param viewHeight The height of the frustum at the near clipping plane.
 * @param nearZ The distance to the near clipping plane.
 * @param farZ The distance to the far clipping plane.
 */
FORCEINLINE void VectorMatrixOrthographicLH(Matrix4f* destinationMatrix, float viewWidth, float viewHeight, float nearZ, float farZ)
{
    using namespace DirectX;
    XMMATRIX result = XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
    XMStoreFloat4x4((XMFLOAT4X4*)destinationMatrix, result);
}

/**
 * Creates a right-handed orthographic projection matrix.
 *
 * @param destinationMatrix Pointer to where the result will be stored.
 * @param viewWidth The width of the frustum at the near clipping plane.
 * @param viewHeight The height of the frustum at the near clipping plane.
 * @param nearZ The distance to the near clipping plane.
 * @param farZ The distance to the far clipping plane.
 */
FORCEINLINE void VectorMatrixOrthographicRH(Matrix4f* destinationMatrix, float viewWidth, float viewHeight, float nearZ, float farZ)
{
    using namespace DirectX;
    XMMATRIX result = XMMatrixOrthographicRH(viewWidth, viewHeight, nearZ, farZ);
    XMStoreFloat4x4((XMFLOAT4X4*)destinationMatrix, result);
}

} // namespace Basalt::Math
} // namespace Basalt
