module;
#include "Libraries.h"
import std;

export module dxvk.app;

import dxvk.backend;

export namespace dxvk {

    class Application
    {
    public:
        Application(UINT width, UINT height, std::wstring name);
        virtual ~Application() = default;

        virtual void OnInit();
        virtual void OnUpdate();
        virtual void OnRender();
        virtual void OnDestroy();

        virtual void OnKeyDown(UINT8 /*key*/)   {}
        virtual void OnKeyUp(UINT8 /*key*/)     {}

        UINT GetWidth() const           { return m_width; }
        UINT GetHeight() const          { return m_height; }
        const WCHAR* GetTitle() const   { return m_title.c_str(); }

    protected:
        std::shared_ptr<backend::RenderingBackEnd> renderingBackEnd;
        std::wstring GetAssetFullPath(LPCWSTR assetName);

        void SetCustomWindowText(LPCWSTR text);

        // Texture
        static constexpr  UINT TextureWidth = 256;
        static constexpr  UINT TextureHeight = 256;
        static constexpr  UINT TexturePixelSize = 4;    // The number of bytes used to represent a pixel in the texture.
        std::vector<UINT8> GenerateTextureData();

        UINT m_width;
        UINT m_height;
        float m_aspectRatio;

    private:
        std::wstring m_assetsPath;
        std::wstring m_title;

        void LoadPipeline();
    };
}