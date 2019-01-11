/***************************************************************************
 *  qgsfeaturepool.cpp                                                     *
 *  -------------------                                                    *
 *  copyright            : (C) 2014 by Sandro Mani / Sourcepole AG         *
 *  email                : smani@sourcepole.ch                             *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsfeaturepool.h"
#include "qgsfeature.h"
#include "qgsfeatureiterator.h"
#include "qgsgeometry.h"
#include "qgsvectorlayer.h"
#include "qgsvectordataprovider.h"
#include "qgsvectorlayerutils.h"
#include "qgsreadwritelocker.h"

#include <QMutexLocker>


QgsFeaturePool::QgsFeaturePool( QgsVectorLayer *layer )
  : mFeatureCache( CACHE_SIZE )
  , mLayer( layer )
  , mLayerId( layer->id() )
  , mGeometryType( layer->geometryType() )
  , mCrs( layer->crs() )
{

}

bool QgsFeaturePool::getFeature( QgsFeatureId id, QgsFeature &feature, QgsFeedback *feedback )
{
  QgsReadWriteLocker locker( mCacheLock, QgsReadWriteLocker::Read );
  QgsFeature *cachedFeature = mFeatureCache.object( id );
  if ( cachedFeature )
  {
    //feature was cached
    feature = *cachedFeature;
  }
  else
  {
    std::unique_ptr<QgsVectorLayerFeatureSource> source = QgsVectorLayerUtils::getFeatureSource( mLayer, feedback );

    // Feature not in cache, retrieve from layer
    // TODO: avoid always querying all attributes (attribute values are needed when merging by attribute)
    if ( !source || !source->getFeatures( QgsFeatureRequest( id ) ).nextFeature( feature ) )
    {
      return false;
    }
    locker.changeMode( QgsReadWriteLocker::Write );
    mFeatureCache.insert( id, new QgsFeature( feature ) );
    mIndex.addFeature( feature );
  }
  return true;
}

QgsFeatureIds QgsFeaturePool::getFeatures( const QgsFeatureRequest &request, QgsFeedback *feedback )
{
  QgsFeatureIds fids;

  std::unique_ptr<QgsVectorLayerFeatureSource> source = QgsVectorLayerUtils::getFeatureSource( mLayer, feedback );

  QgsFeatureIterator it = source->getFeatures( request );
  QgsFeature feature;
  while ( it.nextFeature( feature ) )
  {
    insertFeature( feature );
    fids << feature.id();
  }

  return fids;
}

QgsFeatureIds QgsFeaturePool::allFeatureIds() const
{
  return mFeatureIds;
}

QgsFeatureIds QgsFeaturePool::getIntersects( const QgsRectangle &rect ) const
{
  QgsReadWriteLocker locker( mCacheLock, QgsReadWriteLocker::Read );
  QgsFeatureIds ids = QgsFeatureIds::fromList( mIndex.intersects( rect ) );
  return ids;
}

QgsVectorLayer *QgsFeaturePool::layer() const
{
  Q_ASSERT( QThread::currentThread() == qApp->thread() );

  return mLayer.data();
}

QPointer<QgsVectorLayer> QgsFeaturePool::layerPtr() const
{
  return mLayer;
}

void QgsFeaturePool::insertFeature( const QgsFeature &feature )
{
  QgsReadWriteLocker locker( mCacheLock, QgsReadWriteLocker::Write );
  mFeatureCache.insert( feature.id(), new QgsFeature( feature ) );
  QgsFeature indexFeature( feature );
  mIndex.addFeature( indexFeature );
}

void QgsFeaturePool::refreshCache( const QgsFeature &feature )
{
  QgsReadWriteLocker locker( mCacheLock, QgsReadWriteLocker::Write );
  mFeatureCache.remove( feature.id() );
  mIndex.deleteFeature( feature );
  locker.unlock();

  QgsFeature tempFeature;
  getFeature( feature.id(), tempFeature );
}

void QgsFeaturePool::removeFeature( const QgsFeatureId featureId )
{
  QgsFeature origFeature;
  QgsReadWriteLocker locker( mCacheLock, QgsReadWriteLocker::Unlocked );
  if ( getFeature( featureId, origFeature ) )
  {
    locker.changeMode( QgsReadWriteLocker::Write );
    mIndex.deleteFeature( origFeature );
  }
  locker.changeMode( QgsReadWriteLocker::Write );
  mFeatureCache.remove( origFeature.id() );
}

void QgsFeaturePool::setFeatureIds( const QgsFeatureIds &ids )
{
  mFeatureIds = ids;
}

bool QgsFeaturePool::isFeatureCached( QgsFeatureId fid )
{
  return mFeatureCache.contains( fid );
}

QgsCoordinateReferenceSystem QgsFeaturePool::crs() const
{
  return mCrs;
}

QgsWkbTypes::GeometryType QgsFeaturePool::geometryType() const
{
  return mGeometryType;
}

QString QgsFeaturePool::layerId() const
{
  return mLayerId;
}
