/*
** File: evisgenericeventbrowsergui.h
** Author: Peter J. Ersts ( ersts at amnh.org )
** Creation Date: 2007-03-08
**
** Copyright ( c ) 2007, American Museum of Natural History. All rights reserved.
**
** This library/program is free software; you can redistribute it
** and/or modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or ( at your option ) any later version.
**
** This library/program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** This work was made possible through a grant by the John D. and
** Catherine T. MacArthur Foundation. Additionally, this program was prepared by
** the American Museum of Natural History under award No. NA05SEC46391002
** from the National Oceanic and Atmospheric Administration, U.S. Department
** of Commerce.  The statements, findings, conclusions, and recommendations
** are those of the author( s ) and do not necessarily reflect the views of the
** National Oceanic and Atmospheric Administration or the Department of Commerce.
**
**/
#ifndef eVisGenericEventBrowserGui_H
#define eVisGenericEventBrowserGui_H

#include <QDialog>
#include <QGraphicsPixmapItem>

#include "qgsmaptool.h"
#include "qgsfeature.h"
#include "qgisinterface.h"
#include "qgsvectordataprovider.h"

#include "evisconfiguration.h"

#include "ui_evisgenericeventbrowserguibase.h"

class QgsMapCanvas;

/**
* \class eVisGenericEventBrowserGui
* \brief Generic viewer for browsing event
* The eVisGenericEventBrowserGui is simply a window that will display an image referenced/stored
* in an attribute of a feature. Images can either loaded locally from disk or loaded via a URL.
* The eVisGenericEventBrowserGui also interacts with the map canvas to draw a highlighting symbol
* on the canvas. The highlighting symbol can also be a pointer showing the direction in which the
* image was taken by referencing a compass bearing recorded as an attribute of a feature.
*/
class eVisGenericEventBrowserGui : public QDialog, private Ui::eVisGenericEventBrowserGuiBase
{
    Q_OBJECT

  public:
    //! \brief Constructor called when button is pressed in the plugin toolbar
    eVisGenericEventBrowserGui( QWidget *parent, QgisInterface *interface, Qt::WindowFlags fl );

    //! \brief Constructor called when new browser is requested by the eVisEventIdTool
    eVisGenericEventBrowserGui( QWidget *parent, QgsMapCanvas *canvas, Qt::WindowFlags fl );

    ~eVisGenericEventBrowserGui() override;

  protected:
    void closeEvent( QCloseEvent *event ) override;

  private:
    //Variables
    //! \brief A flag to bypass some signal/slots during gui initialization
    bool mIgnoreEvent = false;

    //! \brief Pointer to the main configurations object
    eVisConfiguration mConfiguration;

    //! \brief Flag indicating if the browser fully initialized
    bool mBrowserInitialized = false;

    //! \brief Index of the attribute field name that closest 'matches' configuration of the parameter
    int mDefaultCompassBearingField = 0;

    //! \brief Index of the attribute field name that closest 'matches' configuration of the parameter
    int mDefaultCompassOffsetField = 0;

    //! \brief Index of the attribute field name that closest 'matches' configuration of the parameter
    int mDefaultEventImagePathField = 0;

    //! \brief Pointer to the QgisInferface
    QgisInterface *mInterface = nullptr;

    //! \brief Pointer to the map canvas
    QgsMapCanvas *mCanvas = nullptr;

    //! \brief Pointer to the vector layer
    QgsVectorLayer *mVectorLayer = nullptr;

    //! \brief Pointer to the vector data provider
    QgsVectorDataProvider *mDataProvider = nullptr;

    //! \brief QPixmap holding the default highlighting symbol
    QPixmap mHighlightSymbol;

    //! \brief QPixmap holding the pointer highlighting symbol
    QPixmap mPointerSymbol;

    //! \brief Compass bearing value for the current feature
    double mCompassBearing = 0;

    //! \brief Compass bearing offset retrieved from attribute
    double mCompassOffset = 0;

    //! \brief QString holding the path to the image for the current feature
    QString mEventImagePath;

    //! \brief List of current select featured ids
    QList<QgsFeatureId> mFeatureIds;

    //! \brief Index of selected feature being viewed, used to access mFeatureIds
    int mCurrentFeatureIndex = 0;

    //! \brief Current feature being viewed
    QgsFeature mFeature;

    //Methods
    //! \brief Applies parameters on the Options tabs and saves the configuration
    void accept() override;

    //! \brief Modifies the Event Image Path according to the local and global settings
    void buildEventImagePath();

    //! \brief Method that loads the image in the browser
    void displayImage();

    //! \brief Generic method to get a feature by id. Access mLocalFeatureList when layer is of type delimitedtext otherwise calls existing methods in mDataProvider
    QgsFeature *featureAtId( QgsFeatureId );

    //! \brief Functionality common to both constructors
    bool initBrowser();

    //! Sets all of the gui objects based on the current configuration
    void initOptionsTab();

    //! \brief Method called to load data into the browser
    void loadRecord();

    //! \brief Reset all gui items on the options tab to a 'system default'
    void restoreDefaultOptions();

    //! Sets the base path to the path of the data source
    void setBasePathToDataSource();

  private slots:
    void launchExternalApplication( QTreeWidgetItem *, int );
    void buttonboxOptions_clicked( QAbstractButton * );
    void chkboxApplyPathRulesToDocs_stateChanged( int );
    void cboxEventImagePathField_currentIndexChanged( int );
    void cboxCompassBearingField_currentIndexChanged( int );
    void cboxCompassOffsetField_currentIndexChanged( int );
    void chkboxDisplayCompassBearing_stateChanged( int );
    void chkboxEventImagePathRelative_stateChanged( int );
    void chkboxUseOnlyFilename_stateChanged( int );
    void displayArea_currentChanged( int );
    void dsboxCompassOffset_valueChanged( double );
    void leBasePath_textChanged( const QString & );
    void pbtnAddFileType_clicked();
    void pbtnDeleteFileType_clicked();
    void pbtnNext_clicked();
    void pbtnPrevious_clicked();
    void pbtnResetApplyPathRulesToDocs_clicked();
    void pbtnResetBasePathData_clicked();
    void pbtnResetCompassBearingData_clicked();
    void pbtnResetCompassOffsetData_clicked();
    void pbtnResetEventImagePathData_clicked();
    void pbtnResetUseOnlyFilenameData_clicked();
    void rbtnManualCompassOffset_toggled( bool );
    void tableFileTypeAssociations_cellDoubleClicked( int, int );
    //! \brief Slot called when the map canvas is done refreshing. Draws the highlighting symbol over the current selected feature
    void renderSymbol( QPainter * );
};
#endif
