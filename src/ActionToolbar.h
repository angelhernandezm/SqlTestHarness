//
// Created by angel on 30/01/2017.
//


#ifndef SQLTESTHARNESS_ACTIONTOOLBAR_H
#define SQLTESTHARNESS_ACTIONTOOLBAR_H

#include "CommonHeaders.h"
#include "GenericCallback.h"
#include "DataContext.h"

using namespace std;
    class ActionToolbar {
    private:
        GtkBuilder *builder;
        GtkWindow* AppWindow_get();
        static DataContext* Context;
        static GtkApplication *application;
        std::map<std::string, GtkWidget*> Buttons;
        std::vector<std::string> ButtonNames_get() const;

    public:
        ActionToolbar();
        void EnableOrDisableButtonsBasedOnMode(bool enable);
        ActionToolbar(GtkApplication &app, const GtkBuilder *builder, DataContext &context);

    protected:
        void Initialize(GtkBuilder *builder);

        /* Event handlers */
        void OnRefresh();
        void OnNewRecord();
        void OnSaveRecord();
        void OnEditRecord();
        void OnCancelEdit();
        void OnNavigateNext();
        void OnNavigateLast();
        void OnDeleteRecord();
        void OnNavigateFirst();
        void OnNavigatePrevious();
    };


#endif //SQLTESTHARNESS_ACTIONTOOLBAR_H


