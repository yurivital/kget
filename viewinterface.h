#ifndef _VIEWINTERFACE_H
#define _VIEWINTERFACE_H

#include <kurl.h>
#include <qvaluelist.h>
#include <qobject.h>

#include "globals.h"

class TransferList;

class ViewInterface : public QObject
{
Q_OBJECT
public:
	ViewInterface( Scheduler * );

signals:
	/**
	 * Those are the commands for the scheduler
     * TEMP(Dario) It's better that TransferList is passed as a reference
     * becouse it gets created and destroyed by the interface (safer).
	 */
	void newURLs( const KURL::List &, const QString &destDir );
	void removeItems( TransferList & );
	void setPriority( TransferList &, int );
	void setOperation( TransferList &, TransferOperation );
	void setGroup( TransferList &, const QString & );

public slots:
	/**
	 * Every slot here is a scheduler notification/answer. Just
	 * reimplement those in a subclass to catch broadcasted
	 * messages.
     * TEMP(Dario) It's better that TransferList is passed as a reference
     * becouse it gets created and destroyed by the scheduler (safer).
	 */
	virtual void schedulerCleared();
	virtual void schedulerAddedItems( TransferList &);
	virtual void schedulerRemovedItems( TransferList &);
	virtual void schedulerChangedItems( TransferList &);
	virtual void schedulerStatus( GlobalStatus * );
};

#endif

