//
// Created by angel on 30/01/2017.
//


#ifndef SQLTESTHARNESS_MAINWINDOWCONTROLLER_H
#define SQLTESTHARNESS_MAINWINDOWCONTROLLER_H

#include "CommonHeaders.h"
#include "BaseController.h"
#include "ActionToolbar.h"
#include "DataContext.h"

using namespace std;
    class MainWindowController : BaseController {
    private:
        void SetUpUI();
        void CreateView();
        GtkBuilder *builder;
        void FetchDataset();
        void ConnectSignals();
        ActionToolbar toolbar;
        GtkTreeModel* FillView();
        static MainWindowController *self;
        static GtkApplication *application;
        void DatabindFields(Contact& contact);
        gboolean UpdateStatusBar(gpointer data);
        std::map<std::string, GtkWidget*> Controls;
        std::vector<std::string> ControlNames_get() const;

    public:
        DataContext Context;
        MainWindowController();
        ~MainWindowController();
        void Refresh(bool ignoreFields = false);
        static void StartUp(GtkApplication &app, gpointer user_data);

    protected:
        void Initialize() override;
        virtual void EnableOrDisableFieldsBasedOnMode(bool enable);
        virtual void ReadOrWriteToFieldsOnUi(UiFieldOperation op, Contact& contact);

        /* Event handlers */
        void OnApplicationQuit(GtkWidget *widget);
        void OnGridSelectionChanged(GtkTreeSelection *selection, gpointer data);
    };

#endif //SQLTESTHARNESS_MAINWINDOWCONTROLLER_H