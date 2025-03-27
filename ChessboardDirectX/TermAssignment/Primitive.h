//
// BGTD 9201
//	Class that represents a simple primitive
//

#include <d3d11_1.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;


class Primitive
{
public:
	Primitive();
	~Primitive();

	void InitializeGeometry(ID3D11Device* pDevice);
	void InitializeInputLayout(ID3D11Device* pDevice, const void* vsBinary, size_t vsBinarySize);
	void Draw(ID3D11DeviceContext* pDeviceContext);


private:
	ID3D11Buffer* pVertexBuffer;
	ID3D11InputLayout* pInputLayout;


};