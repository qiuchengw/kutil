#ifndef kutil_net_h__
#define kutil_net_h__

#pragma once

#include <QSettings>

namespace net {

	__declspec(selectany) QString API_SVR_IP = "";

	inline void setApiServer(const QString& svr) {
		API_SVR_IP = svr;

		QSettings ss;
		ss.beginGroup("svr");
		ss.setValue("svr", API_SVR_IP);
		ss.sync();
	}

	inline QString apiSvr() {
		if (API_SVR_IP.isEmpty()) {
			QSettings ss;
			ss.beginGroup("svr");
			API_SVR_IP = ss.value("svr").toString();
		}
		return API_SVR_IP;
	}

	inline QString urlstr(const QString& api) {
#ifdef _DEBUG
		Q_ASSERT(!apiSvr().isEmpty());
#endif
		return apiSvr() + api;
	}

	inline QUrl url(const QString& api) {
		return QUrl(urlstr(api));
	}
}



#endif // kutil_net_h__
