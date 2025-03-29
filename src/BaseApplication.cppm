module;
#include "Tools.h"

export module dxvk.baseapp;

export namespace dxvk {

    class BaseApplication
    {
    public:
        BaseApplication(UINT width, UINT height, std::wstring name);
        virtual ~BaseApplication() = default;

        virtual void OnInit() = 0;
        virtual void OnUpdate() = 0;
        virtual void OnRender() = 0;
        virtual void OnDestroy() = 0;

        virtual void OnKeyDown(UINT8 /*key*/)   {}
        virtual void OnKeyUp(UINT8 /*key*/)     {}

        UINT GetWidth() const           { return m_width; }
        UINT GetHeight() const          { return m_height; }
        const WCHAR* GetTitle() const   { return m_title.c_str(); }

    protected:
        std::wstring GetAssetFullPath(LPCWSTR assetName);

        void SetCustomWindowText(LPCWSTR text);

        UINT m_width;
        UINT m_height;
        float m_aspectRatio;

    private:
        std::wstring m_assetsPath;
        std::wstring m_title;
    };
}