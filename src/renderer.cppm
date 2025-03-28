module;
#include "D3D12Lite.h"
using namespace D3D12Lite;

export module dx12.rdr;

export namespace dx12 {

    class Renderer {
    public:
        Renderer(HWND windowHandle, Uint2 screenSize)   {
            mDevice = std::make_unique<Device>(windowHandle, screenSize);
            mGraphicsContext = mDevice->CreateGraphicsContext();
        }

        void RenderClearColorTutorial()
        {
            mDevice->BeginFrame();

            TextureResource& backBuffer = mDevice->GetCurrentBackBuffer();

            mGraphicsContext->Reset();

            mGraphicsContext->AddBarrier(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
            mGraphicsContext->FlushBarriers();

            mGraphicsContext->ClearRenderTarget(backBuffer, Color(0.3f, 0.3f, 0.8f));

            mGraphicsContext->AddBarrier(backBuffer, D3D12_RESOURCE_STATE_PRESENT);
            mGraphicsContext->FlushBarriers();

            mDevice->SubmitContextWork(*mGraphicsContext);

            mDevice->EndFrame();
            mDevice->Present();
        }

        void Render()
        {
            RenderClearColorTutorial();
        }

    private:
        std::unique_ptr<Device> mDevice;
        std::unique_ptr<GraphicsContext> mGraphicsContext;
    };

}