#ifndef kutil_net_h__
#define kutil_net_h__

namespace net {

	__declspec(selectany) static QString API_SVR_IP = "";

	inline void setApiServer(const QString& svr) {
		API_SVR_IP = svr;
	}

	inline QString apiSvr() {
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
