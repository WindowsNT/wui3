﻿//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------
#pragma once


namespace winrt::wui3::implementation
{
    using IInspectable = ::winrt::Windows::Foundation::IInspectable;

    template <typename D, typename ... I>
    struct MainWindowT : public ::winrt::wui3::implementation::MainWindow_base<D,
        ::winrt::Microsoft::UI::Xaml::Markup::IComponentConnector,
        I...>
    {
        using base_type = typename MainWindowT::base_type;
        using base_type::base_type;
        using class_type = typename MainWindowT::class_type;

        void InitializeComponent();
        virtual void Connect(int32_t connectionId, IInspectable const& target);
        virtual ::winrt::Microsoft::UI::Xaml::Markup::IComponentConnector GetBindingConnector(int32_t connectionId, IInspectable const& target);
        void UnloadObject(::winrt::Microsoft::UI::Xaml::DependencyObject const& dependencyObject);
        void DisconnectUnloadedObject(int32_t connectionId);

        ::winrt::Microsoft::UI::Xaml::Controls::Button myButton()
        {
            return _myButton;
        }
        void myButton(::winrt::Microsoft::UI::Xaml::Controls::Button value)
        {
            _myButton = value;
        }
        
    protected:
        bool _contentLoaded{false};

    private:
        struct MainWindow_obj1_Bindings;

        ::winrt::Microsoft::UI::Xaml::Controls::Button _myButton{nullptr};
    };
}

