/***************************************************************************
    qgsdashspacedialog.cpp
    ----------------------
    begin                : January 2010
    copyright            : (C) 2010 by Marco Hugentobler
    email                : marco at hugis dot net
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsdashspacedialog.h"
#include "qgsapplication.h"
#include <QFile>

QgsDashSpaceDialog::QgsDashSpaceDialog( const QVector<qreal> &v, QWidget *parent, Qt::WindowFlags f ): QDialog( parent, f )
{
  setupUi( this );
  connect( mAddButton, &QPushButton::clicked, this, &QgsDashSpaceDialog::mAddButton_clicked );
  connect( mRemoveButton, &QPushButton::clicked, this, &QgsDashSpaceDialog::mRemoveButton_clicked );

  mAddButton->setIcon( QgsApplication::getThemeIcon( "symbologyAdd.svg" ) );
  mRemoveButton->setIcon( QgsApplication::getThemeIcon( "symbologyRemove.svg" ) );

  double dash = 0;
  double space = 0;
  for ( int i = 0; i < ( v.size() - 1 ); ++i )
  {
    dash = v.at( i );
    ++i;
    space = v.at( i );
    QTreeWidgetItem *entry = new QTreeWidgetItem();
    entry->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled );
    entry->setText( 0, QString::number( dash ) );
    entry->setText( 1, QString::number( space ) );
    mDashSpaceTreeWidget->addTopLevelItem( entry );
  }
}

void QgsDashSpaceDialog::mAddButton_clicked()
{
  //add new (default) item
  QTreeWidgetItem *entry = new QTreeWidgetItem();
  entry->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled );
  entry->setText( 0, QStringLiteral( "5" ) );
  entry->setText( 1, QStringLiteral( "2" ) );
  mDashSpaceTreeWidget->addTopLevelItem( entry );
}

void QgsDashSpaceDialog::mRemoveButton_clicked()
{
  //get active item
  QTreeWidgetItem *currentItem = mDashSpaceTreeWidget->currentItem();
  if ( currentItem )
  {
    mDashSpaceTreeWidget->takeTopLevelItem( mDashSpaceTreeWidget->indexOfTopLevelItem( currentItem ) );
  }
}

QVector<qreal> QgsDashSpaceDialog::dashDotVector() const
{
  QVector<qreal> dashVector;
  int nTopLevelItems = mDashSpaceTreeWidget->topLevelItemCount();
  for ( int i = 0; i < nTopLevelItems; ++i )
  {
    QTreeWidgetItem *currentItem = mDashSpaceTreeWidget->topLevelItem( i );
    if ( currentItem )
    {
      dashVector << currentItem->text( 0 ).toDouble() << currentItem->text( 1 ).toDouble();
    }
  }
  return dashVector;
}

