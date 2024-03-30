#ifndef DL_DOWNLOADLISTENER_H_
#define DL_DOWNLOADLISTENER_H_

namespace dl {

/*!
 * Interface for monitoring download progress.
 * 
 * \author Malcolm Gill
 * \author Harry Slatyer
 *
 * \copyright see LICENSE
 */
class DownloadListener {
	public:
		virtual ~DownloadListener() {}
		/**
		 * Called when progress is made to the download.
		 * \param dlnow current number of bytes downloaded
		 * \param dltot total size of file to be downloaded
		 */
		virtual void on_progress(double dlnow, double dltot) {}
};

}

#endif
